#include "lua_bind_context.h"
#include "lua_bind.h"
#include "lualib.h"
#include "lauxlib.h"
#include "Context.h"
#include "Effect.h"

static void LOGV(const char* msg) { LFX_LOGV(msg); }
static void LOGD(const char* msg) { LFX_LOGD(msg); }
static void LOGI(const char* msg) { LFX_LOGI(msg); }
static void LOGW(const char* msg) { LFX_LOGW(msg); }
static void LOGE(const char* msg) { LFX_LOGE(msg); }

DEF_FUNC_V_S(LOGV)
DEF_FUNC_V_S(LOGD)
DEF_FUNC_V_S(LOGI)
DEF_FUNC_V_S(LOGW)
DEF_FUNC_V_S(LOGE)

static int lua_LFX_Context_LoadTexture2D(lua_State* L)
{
    LFX_Context* ctx = (LFX_Context*) luaL_checklightuserdata(L, 1);
    const char* path = luaL_checkstringstrict(L, 2);
    LFX_Texture texture = { 0 };
    LFX_PopTexture(L, 3, &texture);
    LFX_RESULT ret = LFX_Context_LoadTexture2D(ctx, path, &texture);
    LFX_PushTexture(L, 3, &texture);
    lua_pushinteger(L, (lua_Integer) ret);
    return 1;
}

static int lua_LFX_Context_CreateTexture(lua_State* L)
{
    LFX_Context* ctx = (LFX_Context*) luaL_checklightuserdata(L, 1);
    LFX_Texture texture = { 0 };
    LFX_PopTexture(L, 2, &texture);
    void* data = lua_touserdata_or_string(L, 3);
    LFX_RESULT ret = LFX_Context_CreateTexture(ctx, &texture, data);
    LFX_PushTexture(L, 2, &texture);
    lua_pushinteger(L, (lua_Integer) ret);
    return 1;
}

static int lua_LFX_Context_DestroyTexture(lua_State* L)
{
    LFX_Context* ctx = (LFX_Context*) luaL_checklightuserdata(L, 1);
    LFX_Texture texture = { 0 };
    LFX_PopTexture(L, 2, &texture);
    LFX_RESULT ret = LFX_Context_DestroyTexture(ctx, &texture);
    lua_pushinteger(L, (lua_Integer) ret);
    return 1;
}

static int lua_LFX_Context_RenderQuad(lua_State* L)
{
    LFX_Context* ctx = (LFX_Context*) luaL_checklightuserdata(L, 1);
    LFX_Texture texture = { 0 };
    LFX_PopTexture(L, 2, &texture);
    float* matrix = (float*) lua_touserdata_or_string(L, 3);
    LFX_RESULT ret = LFX_Context_RenderQuad(ctx, &texture, matrix);
    lua_pushinteger(L, (lua_Integer) ret);
    return 1;
}

static void LoadFileAsyncCallback(LFX_RESULT ret, LFX_Context* thiz, const char* path, void* data, int size, void* user_data)
{
    LuaCallbackContext* context = user_data;
    lua_State* L = context->L;
    lua_rawgeti(L, LUA_REGISTRYINDEX, context->callback_ref);
    free(context);

    lua_pushinteger(L, ret);
    lua_pushlightuserdata(L, thiz);
    lua_pushstring(L, path);
    lua_pushlightuserdata(L, data);
    lua_pushinteger(L, size);

    int lua_ret = lua_pcall(L, 5, 0, 0);
    if (lua_ret != LUA_OK)
    {
        size_t len = 0;
        const char* err = lua_tolstring(L, -1, &len);
        if (err && len > 0)
        {
            LFX_LOGE(err);
        }
        lua_pop(L, 1);
        LFX_LOGE("LFX_LUA_RUN_ERROR");
    }
}

static int lua_LFX_Context_LoadFileAsync(lua_State* L)
{
    LFX_Context* ctx = (LFX_Context*) luaL_checklightuserdata(L, 1);
    const char* path = luaL_checkstringstrict(L, 2);
    if (lua_isfunction(L, 3))
    {
        lua_pushvalue(L, 3);
        int callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);

        LuaCallbackContext* context = malloc(sizeof(LuaCallbackContext));
        context->L = L;
        context->callback_ref = callback_ref;

        LFX_Context_LoadFileAsync(ctx, path, LoadFileAsyncCallback, context);
    }
    else
    {
        luaL_argerror(L, 3, "lua function expected");
    }
    return 0;
}

DEF_FUNC_I_ASSA(LFX_Context_CreateProgram)
DEF_FUNC_I_ASA(LFX_Context_GetFileSize)
DEF_FUNC_I_ASAA(LFX_Context_LoadFile)
DEF_FUNC_I_AS(LFX_Context_CheckGLExtension)
DEF_FUNC_I_AAAA(LFX_Context_GetGLVersion)
DEF_FUNC_I_AA(LFX_Context_GetBuildPlatform)

static const luaL_Reg context_funcs[] = {
    REG_FUNC(LOGV),
    REG_FUNC(LOGD),
    REG_FUNC(LOGI),
    REG_FUNC(LOGW),
    REG_FUNC(LOGE),
    REG_FUNC(LFX_Context_LoadTexture2D),
    REG_FUNC(LFX_Context_CreateTexture),
    REG_FUNC(LFX_Context_DestroyTexture),
    REG_FUNC(LFX_Context_RenderQuad),
    REG_FUNC(LFX_Context_CreateProgram),
    REG_FUNC(LFX_Context_GetFileSize),
    REG_FUNC(LFX_Context_LoadFile),
    REG_FUNC(LFX_Context_LoadFileAsync),
    REG_FUNC(LFX_Context_CheckGLExtension),
    REG_FUNC(LFX_Context_GetGLVersion),
    REG_FUNC(LFX_Context_GetBuildPlatform),
    { NULL, NULL }
};

static const DefineReg context_defines[] = {
    // LFX_RESULT
    REG_DEF(LFX_SUCCESS),
    REG_DEF(LFX_FAIL),
    REG_DEF(LFX_INVALID_CONTEXT),
    REG_DEF(LFX_INVALID_INPUT),
    REG_DEF(LFX_IMAGE_LOAD_FAIL),
    REG_DEF(LFX_SHADER_COMPILE_ERROR),
    REG_DEF(LFX_PROGRAM_LINK_ERROR),
    REG_DEF(LFX_GL_ERROR),
    REG_DEF(LFX_INVALID_EFFECT),
    REG_DEF(LFX_FILE_OPEN_FAIL),
    REG_DEF(LFX_FILE_READ_ERROR),
    REG_DEF(LFX_LUA_LOAD_ERROR),
    REG_DEF(LFX_LUA_NO_FUNCTION),
    REG_DEF(LFX_LUA_RUN_ERROR),
    REG_DEF(LFX_INVLAID_GL_CONTEXT),
    REG_DEF(LFX_UNKNOWN_GL_VERSION),
    REG_DEF(LFX_NOT_SUPPORT_GL_VERSION),
    REG_DEF(LFX_MESSAGE_REPLY_BUFFER_NOT_ENOUGH),
    REG_DEF(LFX_JNI_ENV_ERROR),
    REG_DEF(LFX_ASSET_OPEN_FAIL),
    REG_DEF(LFX_NO_IMPLEMENT),

    // LFX_BUILD_PLATFORM
    REG_DEF(LFX_BUILD_PLATFORM_WINDOWS),
    REG_DEF(LFX_BUILD_PLATFORM_ANDROID),
    REG_DEF(LFX_BUILD_PLATFORM_MAC),
    REG_DEF(LFX_BUILD_PLATFORM_IOS),
    REG_DEF(LFX_BUILD_PLATFORM_WASM),

    // LFX_MESSAGE_ID
    REG_DEF(LFX_MESSAGE_ID_SET_EFFECT_TIMESTAMP),
};

void LFX_LuaBindContext(lua_State* L)
{
    lua_pushglobaltable(L);
    luaL_setfuncs(L, context_funcs, 0);

    luaL_setintfields(L, context_defines, sizeof(context_defines) / sizeof(context_defines[0]));

    lua_pop(L, 1);
}
