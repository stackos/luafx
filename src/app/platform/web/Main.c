#include "gl_include.h"
#include "luafx.h"
#include "Context.h"
#include "cJSON.h"
#include <emscripten.h>

static int g_width = 0;
static int g_height = 0;
static int g_context = LFX_INVALID_HANDLE;
static int g_effect = LFX_INVALID_HANDLE;
static LFX_Texture g_texture_in;
static LFX_Texture g_texture_out;
static char* g_message_buffer = NULL;
static size_t g_message_buffer_size = 0;
static int64_t g_fps_update_time = 0;
static int g_frame_count = 0;
static int64_t g_start_time = 0;

void* EMSCRIPTEN_KEEPALIVE MemoryAlloc(int size)
{
    return malloc(size);
}

void EMSCRIPTEN_KEEPALIVE MemoryFree(void* p)
{
    free(p);
}

void EMSCRIPTEN_KEEPALIVE InitEngine(const char* msg)
{
    LFX_LOGI("InitEngine: %s", msg);

    cJSON* json = cJSON_Parse(msg);
    if (json == NULL)
    {
        return;
    }

    g_width = (int) cJSON_GetObjectItem(json, "width")->valuedouble;
    g_height = (int) cJSON_GetObjectItem(json, "height")->valuedouble;
    int64_t time = (int64_t) cJSON_GetObjectItem(json, "time")->valuedouble;
    cJSON_Delete(json);

    LFX_CreateContext(&g_context);

    LFX_LoadEffect(g_context, "./assets/effect.lua", &g_effect);

    LFX_LoadTexture2D(g_context, "./assets/input/2560x1600.jpg", &g_texture_in);

    memset(&g_texture_out, 0, sizeof(g_texture_out));
    g_texture_out.target = GL_TEXTURE_2D;
    g_texture_out.format = GL_RGBA;
    g_texture_out.width = g_texture_in.width;
    g_texture_out.height = g_texture_in.height;
    g_texture_out.filter_mode = GL_LINEAR;
    g_texture_out.wrap_mode = GL_CLAMP_TO_EDGE;
    LFX_CreateTexture(g_context, &g_texture_out);

    // message buffer
    g_message_buffer_size = 1024;
    g_message_buffer = (char*) malloc(g_message_buffer_size);

    g_start_time = time;
}

void EMSCRIPTEN_KEEPALIVE DoneEngine(const char* msg)
{
    free(g_message_buffer);
    LFX_DestroyTexture(g_context, &g_texture_in);
    LFX_DestroyTexture(g_context, &g_texture_out);
    LFX_DestroyEffect(g_context, g_effect);
    LFX_DestroyContext(g_context);
}

void EMSCRIPTEN_KEEPALIVE UpdateEngine(const char* msg)
{
    cJSON* json = cJSON_Parse(msg);
    if (json == NULL)
    {
        return;
    }

    int64_t time = (int64_t) cJSON_GetObjectItem(json, "time")->valuedouble;
    cJSON_Delete(json);

    sprintf(g_message_buffer, "{\"timestamp\": %lld}", time);

    LFX_SendEffectMessage(g_context, g_effect, LFX_MESSAGE_ID_SET_EFFECT_TIMESTAMP, g_message_buffer, NULL, 0);

    LFX_RenderEffect(g_context, g_effect, &g_texture_in, &g_texture_out, NULL);

    glViewport(0, 0, g_width, g_height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    LFX_RenderQuad(g_context, &g_texture_out, NULL);

    int64_t t = time;
    if (t - g_fps_update_time > 1000)
    {
        int fps = g_frame_count;
        g_frame_count = 0;
        g_fps_update_time = t;

        LFX_LOGI("fps: %d", fps);
    }
    g_frame_count += 1;
}

int main()
{
    return 0;
}
