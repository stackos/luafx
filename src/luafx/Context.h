#pragma once

#include "luafx.h"
#include "ObjectPool.h"
#include "Vector.h"

typedef int LFX_LOG_LEVEL;
#define LFX_VERBOSE 0
#define LFX_DEBUG 1
#define LFX_INFO 2
#define LFX_WARN 3
#define LFX_ERROR 4

#define LFX_LOGV(...) LFX_Log(LFX_VERBOSE, __VA_ARGS__)
#define LFX_LOGD(...) LFX_Log(LFX_DEBUG, __VA_ARGS__)
#define LFX_LOGI(...) LFX_Log(LFX_INFO, __VA_ARGS__)
#define LFX_LOGW(...) LFX_Log(LFX_WARN, __VA_ARGS__)
#define LFX_LOGE(...) LFX_Log(LFX_ERROR, __VA_ARGS__)

#define RETURN_ERR(err) LFX_LOGE(#err); return err;

LFX_RESULT LFX_Log(LFX_LOG_LEVEL level, const char* format, ...);

// class
typedef struct LFX_Context
{
    int quad_vbo;
    int copy_program;
    LFX_ObjectPool effect_pool;
    int gl_es;
    char gl_version[2];
    char** gl_extensions;
    int gl_num_extensions;
    int default_vao;
} LFX_Context;

// public
LFX_RESULT LFX_Context_Init(LFX_Context* thiz);
void LFX_Context_Done(LFX_Context* thiz);

LFX_RESULT LFX_Context_LoadTexture2D(LFX_Context* thiz, const char* path, LFX_Texture* texture);
LFX_RESULT LFX_Context_CreateTexture(LFX_Context* thiz, LFX_Texture* texture, const void* data);
LFX_RESULT LFX_Context_DestroyTexture(LFX_Context* thiz, const LFX_Texture* texture);

LFX_RESULT LFX_Context_RenderQuad(LFX_Context* thiz, const LFX_Texture* texture, const float* matrix);

LFX_RESULT LFX_Context_LoadEffect(LFX_Context* thiz, const char* path, int* effect);
LFX_RESULT LFX_Context_DestroyEffect(LFX_Context* thiz, int effect);
LFX_RESULT LFX_Context_RenderEffect(LFX_Context* thiz, int effect, const LFX_Texture* input_texture, const LFX_Texture* output_texture, void* output_image);
LFX_RESULT LFX_Context_SendEffectMessage(LFX_Context* thiz, int effect, LFX_MESSAGE_ID message_id, const char* message, char* reply_buffer, int reply_buffer_size);
LFX_RESULT LFX_Context_SetEffectMessageCallback(LFX_Context* thiz, int effect, LFX_EFFECT_MESSAGE_CALLBACK callback);

// private
void LFX_Context_CreateQuadVBO(LFX_Context* thiz);
void LFX_Context_CreateCopyProgram(LFX_Context* thiz);
LFX_RESULT LFX_Context_CreateShader(LFX_Context* thiz, const char* source, int type, int* shader);
void LFX_Context_NormalizePath(LFX_Context* thiz, char* path);

// internal
LFX_RESULT LFX_Context_CreateProgram(LFX_Context* thiz, const char* vs, const char* fs, int* program);
LFX_RESULT LFX_Context_GetFileSize(LFX_Context* thiz, const char* path, int* size);
LFX_RESULT LFX_Context_LoadFile(LFX_Context* thiz, const char* path, void** data, int* size);
LFX_RESULT LFX_Context_CheckGLExtension(LFX_Context* thiz, const char* name);
LFX_RESULT LFX_Context_GetGLVersion(LFX_Context* thiz, int* major, int* minor, int* is_es);
