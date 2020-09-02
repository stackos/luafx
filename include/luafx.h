#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>

#if defined(_WIN32)

#ifdef LFX_STATIC
#  define LFX_API extern
#else
#  ifdef LFX_BUILD
#    define LFX_API extern __declspec(dllexport)
#  else
#    define LFX_API extern __declspec(dllimport)
#  endif
#endif

#else

#ifdef LFX_STATIC
#  define LFX_API extern
#else
#  if defined(__GNUC__) && __GNUC__>=4
#   define LFX_API extern __attribute__ ((visibility("default")))
#  elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#   define LFX_API extern __global
#  else
#   define LFX_API extern
#  endif
#endif

#endif

#define LFX_VERSION_NAME "1.0"
#define LFX_VERSION_MAJOR 1
#define LFX_VERSION_MINOR 0
#define LFX_MAKE_VERSION(major, minor) ((major << 16) | minor)
#define LFX_VERSION LFX_MAKE_VERSION(LFX_VERSION_MAJOR, LFX_VERSION_MINOR)

#define LFX_INVALID_HANDLE 0

typedef int LFX_RESULT;
#define LFX_SUCCESS 0
#define LFX_FAIL 1
#define LFX_INVALID_CONTEXT 2
#define LFX_INVALID_INPUT 3
#define LFX_IMAGE_LOAD_FAIL 4
#define LFX_SHADER_COMPILE_ERROR 5
#define LFX_PROGRAM_LINK_ERROR 6
#define LFX_GL_ERROR 7
#define LFX_INVALID_EFFECT 8
#define LFX_FILE_OPEN_FAIL 9
#define LFX_FILE_READ_ERROR 10
#define LFX_LUA_LOAD_ERROR 11
#define LFX_LUA_NO_FUNCTION 12
#define LFX_LUA_RUN_ERROR 13
#define LFX_INVLAID_GL_CONTEXT 14
#define LFX_UNKNOWN_GL_VERSION 15
#define LFX_NOT_SUPPORT_GL_VERSION 16 // min support version is gl 3.0 and gles 2.0
#define LFX_MESSAGE_REPLY_BUFFER_NOT_ENOUGH 17
#define LFX_JNI_ENV_ERROR 18
#define LFX_ASSET_OPEN_FAIL 19
#define LFX_NO_IMPLEMENT 20

typedef int LFX_BUILD_PLATFORM;
#define LFX_BUILD_PLATFORM_WINDOWS 0
#define LFX_BUILD_PLATFORM_ANDROID 1
#define LFX_BUILD_PLATFORM_MAC 2
#define LFX_BUILD_PLATFORM_IOS 3
#define LFX_BUILD_PLATFORM_WASM 4

typedef int LFX_MESSAGE_ID;
/*
LFX_MESSAGE_ID_SET_EFFECT_TIMESTAMP message content sample:
{
    "timestamp": 0
}
*/
#define LFX_MESSAGE_ID_SET_EFFECT_TIMESTAMP -1
#define LFX_MESSAGE_ID_MOUSE_DOWN -2
#define LFX_MESSAGE_ID_MOUSE_UP -3
#define LFX_MESSAGE_ID_MOUSE_MOVE -4

typedef LFX_RESULT (*LFX_EFFECT_MESSAGE_CALLBACK)(LFX_MESSAGE_ID message_id, const char* message, char* reply_buffer, int reply_buffer_size);
typedef void (*LFX_LOG_CALLBACK)(const char* message);

typedef struct LFX_Texture
{
    int id;
    int target;
    int format;
    int width;
    int height;
    int filter_mode;
    int wrap_mode;
} LFX_Texture;

#define LFX_MAX_PATH 512
typedef char LFX_Path[LFX_MAX_PATH];

#ifdef __cplusplus
extern "C" {
#endif

LFX_API LFX_RESULT LFX_CreateContext(int* context);
LFX_API LFX_RESULT LFX_DestroyContext(int context);

LFX_API LFX_RESULT LFX_LoadTexture2D(int context, const char* path, LFX_Texture* texture);
LFX_API LFX_RESULT LFX_CreateTexture(int context, LFX_Texture* texture);
LFX_API LFX_RESULT LFX_DestroyTexture(int context, const LFX_Texture* texture);

LFX_API LFX_RESULT LFX_RenderQuad(int context, const LFX_Texture* texture, const float* matrix);

LFX_API LFX_RESULT LFX_LoadEffect(int context, const char* path, int* effect);
LFX_API LFX_RESULT LFX_DestroyEffect(int context, int effect);
LFX_API LFX_RESULT LFX_RenderEffect(int context, int effect, const LFX_Texture* input_texture, const LFX_Texture* output_texture, void* output_image);
LFX_API LFX_RESULT LFX_SendEffectMessage(int context, int effect, LFX_MESSAGE_ID message_id, const char* message, char* reply_buffer, int reply_buffer_size);
LFX_API LFX_RESULT LFX_SetEffectMessageCallback(int context, int effect, LFX_EFFECT_MESSAGE_CALLBACK callback);

LFX_API LFX_RESULT LFX_SetLogCallback(LFX_LOG_CALLBACK callback);

#ifdef __cplusplus
}
#endif
