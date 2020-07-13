#pragma once

#include "luafx.h"
#include "Context.h"

// class
typedef struct LFX_Effect
{
    LFX_Context* context;
    struct lua_State* lua_state;
    LFX_Path lua_path;
    LFX_Path effect_dir;
    LFX_EFFECT_MESSAGE_CALLBACK message_callback;
    int fbo;
    int support_pbo;
    int pbo_read[2];
    int pbo_size[2];
    int pbo_index;
} LFX_Effect;

// public
void LFX_Effect_Init(LFX_Effect* thiz, struct LFX_Context* context);
void LFX_Effect_Done(LFX_Effect* thiz);
LFX_RESULT LFX_Effect_Load(LFX_Effect* thiz, const char* path);
LFX_RESULT LFX_Effect_Render(LFX_Effect* thiz, const LFX_Texture* input_texture, const LFX_Texture* output_texture, void* output_image);
LFX_RESULT LFX_Effect_SendMessage(LFX_Effect* thiz, LFX_MESSAGE_ID message_id, const char* message, char* reply_buffer, int reply_buffer_size);
LFX_RESULT LFX_Effect_SetMessageCallback(LFX_Effect* thiz, LFX_EFFECT_MESSAGE_CALLBACK callback);

// private
LFX_RESULT LFX_Effect_GetLuaFunction(LFX_Effect* thiz, const char* name);
LFX_RESULT LFX_Effect_CallLuaFunction(LFX_Effect* thiz, int arg_count);
LFX_RESULT LFX_Effect_GetLuaFunctionIntResult(LFX_Effect* thiz);
LFX_RESULT LFX_Effect_GetThenCallLuaFunction(LFX_Effect* thiz, const char* name);
void LFX_Effect_ReadTexture(LFX_Effect* thiz, const LFX_Texture* output_texture, void* output_image);
void LFX_PushTexture(struct lua_State* L, int index, const LFX_Texture* texture);
void LFX_PopTexture(struct lua_State* L, int index, LFX_Texture* texture);

// internal
const char* LFX_Effect_GetEffectDir(LFX_Effect* thiz);
const char* LFX_Effect_MessageCallback(LFX_Effect* thiz, LFX_MESSAGE_ID message_id, const char* message);
