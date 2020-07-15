#include "luafx.h"
#include "Context.h"
#include "ObjectPool.h"

#ifdef LFX_WINDOWS
#include <Windows.h>
#endif

#ifdef LFX_ANDROID
#include <android/log.h>
#endif

#define MAX_LOG_BUFFER_SIZE 2048
static char g_log_buffer[MAX_LOG_BUFFER_SIZE];
static LFX_LOG_CALLBACK g_log_callback;

#define CHECK_CONTEXT(context) \
    LFX_Context* ctx = (LFX_Context*) LFX_ObjectPool_GetObject(&g_context_pool, context); \
    if (ctx == NULL) { RETURN_ERR(LFX_INVALID_CONTEXT); }

static LFX_ObjectPool g_context_pool = {
    NULL, 0, 0, sizeof(LFX_Context)
};

LFX_RESULT LFX_CreateContext(int* context)
{
    *context = LFX_ObjectPool_CreateObject(&g_context_pool);
    CHECK_CONTEXT(*context);
    LFX_RESULT ret = LFX_Context_Init(ctx);
    if (ret != LFX_SUCCESS)
    {
        LFX_Context_Done(ctx);
        LFX_ObjectPool_DestroyObject(&g_context_pool, *context);
        *context = 0;
        return ret;
    }
    return LFX_SUCCESS;
}

LFX_RESULT LFX_DestroyContext(int context)
{
    CHECK_CONTEXT(context);
    LFX_Context_Done(ctx);
    LFX_ObjectPool_DestroyObject(&g_context_pool, context);
    return LFX_SUCCESS;
}

LFX_RESULT LFX_LoadTexture2D(int context, const char* path, LFX_Texture* texture)
{
    CHECK_CONTEXT(context);
    return LFX_Context_LoadTexture2D(ctx, path, texture);
}

LFX_RESULT LFX_CreateTexture(int context, LFX_Texture* texture)
{
    CHECK_CONTEXT(context);
    return LFX_Context_CreateTexture(ctx, texture, NULL);
}

LFX_RESULT LFX_DestroyTexture(int context, const LFX_Texture* texture)
{
    CHECK_CONTEXT(context);
    return LFX_Context_DestroyTexture(ctx, texture);
}

LFX_RESULT LFX_RenderQuad(int context, const LFX_Texture* texture, const float* matrix)
{
    CHECK_CONTEXT(context);
    return LFX_Context_RenderQuad(ctx, texture, matrix);
}

LFX_RESULT LFX_LoadEffect(int context, const char* path, int* effect)
{
    CHECK_CONTEXT(context);
    return LFX_Context_LoadEffect(ctx, path, effect);
}

LFX_RESULT LFX_DestroyEffect(int context, int effect)
{
    CHECK_CONTEXT(context);
    return LFX_Context_DestroyEffect(ctx, effect);
}

LFX_RESULT LFX_RenderEffect(int context, int effect, const LFX_Texture* input_texture, const LFX_Texture* output_texture, void* output_image)
{
    CHECK_CONTEXT(context);
    return LFX_Context_RenderEffect(ctx, effect, input_texture, output_texture, output_image);
}

LFX_RESULT LFX_SendEffectMessage(int context, int effect, LFX_MESSAGE_ID message_id, const char* message, char* reply_buffer, int reply_buffer_size)
{
    CHECK_CONTEXT(context);
    return LFX_Context_SendEffectMessage(ctx, effect, message_id, message, reply_buffer, reply_buffer_size);
}

LFX_RESULT LFX_SetEffectMessageCallback(int context, int effect, LFX_EFFECT_MESSAGE_CALLBACK callback)
{
    CHECK_CONTEXT(context);
    return LFX_Context_SetEffectMessageCallback(ctx, effect, callback);
}

LFX_RESULT LFX_SetLogCallback(LFX_LOG_CALLBACK callback)
{
    g_log_callback = callback;
    return LFX_SUCCESS;
}

LFX_RESULT LFX_Log(LFX_LOG_LEVEL level, const char* format, ...)
{
    if (level < LFX_VERBOSE || level > LFX_ERROR || format == NULL)
    {
        return LFX_INVALID_INPUT;
    }

    va_list args;
    va_start(args, format);
    vsnprintf(g_log_buffer, MAX_LOG_BUFFER_SIZE, format, args);
    va_end(args);

    if (g_log_callback)
    {
        g_log_callback(g_log_buffer);
    }

#if defined(LFX_WINDOWS)
    HANDLE std_out = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD attribute = 0;
    char* tag = NULL;
        
    switch (level)
    {
        case LFX_VERBOSE:
            attribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            tag = "LFX_LOGV";
            break;

        case LFX_DEBUG:
            attribute = BACKGROUND_GREEN | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            tag = "LFX_LOGD";
            break;

        case LFX_INFO:
            attribute = BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            tag = "LFX_LOGI";
            break;

        case LFX_WARN:
            attribute = BACKGROUND_RED | BACKGROUND_GREEN | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            tag = "LFX_LOGW";
            break;

        case LFX_ERROR:
            attribute = BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            tag = "LFX_LOGE";
            break;

        default:
            break;
    }

    SetConsoleTextAttribute(std_out, attribute);
    printf("[%s]: %s\n", tag, g_log_buffer);

    // restore default
    SetConsoleTextAttribute(std_out, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#elif defined(LFX_ANDROID)
    switch (level)
    {
        case LFX_VERBOSE:
            __android_log_write(ANDROID_LOG_VERBOSE, "LFX_LOGV", g_log_buffer);
            break;

        case LFX_DEBUG:
            __android_log_write(ANDROID_LOG_DEBUG, "LFX_LOGD", g_log_buffer);
            break;

        case LFX_INFO:
            __android_log_write(ANDROID_LOG_INFO, "LFX_LOGI", g_log_buffer);
            break;

        case LFX_WARN:
            __android_log_write(ANDROID_LOG_WARN, "LFX_LOGW", g_log_buffer);
            break;

        case LFX_ERROR:
            __android_log_write(ANDROID_LOG_ERROR, "LFX_LOGE", g_log_buffer);
            break;

        default:
            break;
    }
#elif defined(LFX_MAC) || defined(LFX_IOS) || defined(LFX_WASM)
    char* tag = NULL;
        
    switch (level)
    {
        case LFX_VERBOSE:
            tag = "LFX_LOGV";
            break;

        case LFX_DEBUG:
            tag = "LFX_LOGD";
            break;

        case LFX_INFO:
            tag = "LFX_LOGI";
            break;

        case LFX_WARN:
            tag = "LFX_LOGW";
            break;

        case LFX_ERROR:
            tag = "LFX_LOGE";
            break;

        default:
            break;
    }
    printf("[%s]: %s\n", tag, g_log_buffer);
#else
    not implement.
#endif

    return LFX_SUCCESS;
}
