#include "Effect.h"
#include "gl_include.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lua_bind/lua_bind_gl.h"
#include "lua_bind/lua_bind_context.h"
#include "lua_bind/lua_bind_effect.h"
#include "lua_bind/lua_bind_cglm.h"
#include "lua_bind/lua_bind_memory.h"
#include "lua_bind/lua_bind_cjson.h"
#include "cJSON.h"
#include "Vector.h"

#define LUA_EFFECT_FUNC_NAME_LOAD "Effect_Load"
#define LUA_EFFECT_FUNC_NAME_DONE "Effect_Done"
#define LUA_EFFECT_FUNC_NAME_RENDER "Effect_Render"
#define LUA_EFFECT_FUNC_NAME_SENDMESSAGE "Effect_SendMessage"

#define MESSAGE_BUFFER_SIZE 8192
static char g_message_buffer[MESSAGE_BUFFER_SIZE];

static void AddPackagePath(lua_State* L, const char* path)
{
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path"); // get field "path" from table at top of stack (-1)
    const char* old_path = lua_tostring(L, -1); // grab path string from top of stack
    size_t size = strlen(old_path) + 1 + strlen(path) + 1;
    char* new_path = malloc(size);
    strcpy(new_path, old_path);
    strcat(new_path, ";");
    strcat(new_path, path);
    lua_pop(L, 1); // get rid of the string on the stack we just pushed on line 5
    lua_pushstring(L, new_path); // push the new one
    lua_setfield(L, -2, "path"); // set the field "path" in table at -2 with value at top of stack
    lua_pop(L, 1); // get rid of package table from top of stack
    free(new_path);
}

void LFX_Effect_Init(LFX_Effect* thiz, LFX_Context* context)
{
    memset(thiz, 0, sizeof(LFX_Effect));

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    LFX_LuaBindGL(L);
    LFX_LuaBindContext(L);
    LFX_LuaBindEffect(L);
    LFX_LuaBindCGLM(L);
    LFX_LuaBindMemory(L);
    LFX_LuaBindCJSON(L);

    thiz->context = context;
    thiz->lua_state = L;

    glGenFramebuffers(1, (GLuint*) &thiz->fbo);

    int major = 0;
    int minor = 0;
    int is_es = 0;
    LFX_Context_GetGLVersion(thiz->context, &major, &minor, &is_es);
    
    thiz->support_pbo = (major >= 3);
    if (thiz->support_pbo)
    {
        glGenBuffers(2, (GLuint*) thiz->pbo_read);
    }
}

void LFX_Effect_Done(LFX_Effect* thiz)
{
    LFX_Effect_GetThenCallLuaFunction(thiz, LUA_EFFECT_FUNC_NAME_DONE);

    lua_State* L = thiz->lua_state;
    lua_close(L);

    thiz->lua_state = NULL;

    glDeleteFramebuffers(1, (GLuint*) &thiz->fbo);

    if (thiz->support_pbo)
    {
        glDeleteBuffers(2, (GLuint*) thiz->pbo_read);
    }
}

LFX_RESULT LFX_Effect_Load(LFX_Effect* thiz, const char* path)
{
    if (path == NULL)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    strcpy(thiz->lua_path, path);

    // set effect dir
    {
        strcpy(thiz->effect_dir, path);
        size_t len = strrchr(thiz->effect_dir, '/') - thiz->effect_dir;
        thiz->effect_dir[len] = 0;
    }

    // load lua file
    char* lua_data = NULL;
    int lua_data_size = 0;

    LFX_RESULT ret = LFX_Context_LoadFile(thiz->context, thiz->lua_path, (void*) &lua_data, &lua_data_size);
    if (ret != LFX_SUCCESS)
    {
        return ret;
    }

    if (lua_data == NULL)
    {
        RETURN_ERR(LFX_LUA_LOAD_ERROR);
    }

    lua_State* L = thiz->lua_state;

    // add package path
    LFX_Path package_path;
    strcpy(package_path, thiz->effect_dir);
    strcat(package_path, "/?.lua");
    AddPackagePath(L, package_path);
    strcpy(package_path, thiz->effect_dir);
    strcat(package_path, "/lua_lib/?.lua");
    AddPackagePath(L, package_path);

    // load lua string buffer
    int lua_ret = luaL_loadbuffer(L, lua_data, lua_data_size, path);
    free(lua_data);

    if (lua_ret != LUA_OK)
    {
        RETURN_ERR(LFX_LUA_LOAD_ERROR);
    }

    // call lua
    lua_ret = lua_pcall(L, 0, 0, 0);
    if (lua_ret != LUA_OK)
    {
        size_t len = 0;
        const char* err = lua_tolstring(L, -1, &len);
        if (err && len > 0)
        {
            LFX_LOGE(err);
        }
        lua_pop(L, 1);
        RETURN_ERR(LFX_LUA_RUN_ERROR);
    }

    // call Effect_Load
    ret = LFX_Effect_GetThenCallLuaFunction(thiz, LUA_EFFECT_FUNC_NAME_LOAD);
    if (ret != LFX_SUCCESS)
    {
        return ret;
    }

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Effect_Render(LFX_Effect* thiz, const LFX_Texture* input_texture, const LFX_Texture* output_texture, void* output_image)
{
    if (input_texture == NULL || output_texture == NULL)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    int err = glGetError();
    if (err)
    {
        LFX_LOGW("Before render effect glGetError:%d", err);
    }

    int old_fbo = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, thiz->fbo);

    LFX_RESULT ret = LFX_Effect_GetLuaFunction(thiz, LUA_EFFECT_FUNC_NAME_RENDER);
    if (ret != LFX_SUCCESS)
    {
        return ret;
    }

    lua_State* L = thiz->lua_state;

    lua_pushlightuserdata(L, thiz->context);
    lua_pushlightuserdata(L, thiz);
    lua_newtable(L);
    LFX_PushTexture(L, -1, input_texture);
    lua_newtable(L);
    LFX_PushTexture(L, -1, output_texture);

    ret = LFX_Effect_CallLuaFunction(thiz, 4);
    if (ret != LFX_SUCCESS)
    {
        return ret;
    }

    ret = LFX_Effect_GetLuaFunctionIntResult(thiz);
    if (ret != LFX_SUCCESS)
    {
        return ret;
    }

    if (output_image)
    {
        LFX_Effect_ReadTexture(thiz, output_texture, output_image);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, old_fbo);

    err = glGetError();
    if (err)
    {
        LFX_LOGW("After render effect glGetError:%d", err);
    }

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Effect_SendMessage(LFX_Effect* thiz, LFX_MESSAGE_ID message_id, const char* message, char* reply_buffer, int reply_buffer_size)
{
    if (message == NULL || (reply_buffer == NULL && reply_buffer_size > 0))
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_RESULT ret = LFX_Effect_GetLuaFunction(thiz, LUA_EFFECT_FUNC_NAME_SENDMESSAGE);
    if (ret != LFX_SUCCESS)
    {
        return ret;
    }

    lua_State* L = thiz->lua_state;

    lua_pushlightuserdata(L, thiz->context);
    lua_pushlightuserdata(L, thiz);
    lua_pushinteger(L, (lua_Integer) message_id);
    lua_pushstring(L, message);

    ret = LFX_Effect_CallLuaFunction(thiz, 4);
    if (ret != LFX_SUCCESS)
    {
        return ret;
    }

    const char* reply = NULL;
    size_t reply_len = 0;
    if (lua_type(L, -1) == LUA_TSTRING)
    {
        reply = luaL_checklstring(L, -1, &reply_len);
    }
    lua_pop(L, 1);

    if (reply_buffer_size > 0)
    {
        if (reply != NULL && reply_len > 0)
        {
            if (reply_len < (size_t) reply_buffer_size)
            {
                memcpy(reply_buffer, reply, reply_len);
                reply_buffer[reply_len] = 0;
            }
            else
            {
                RETURN_ERR(LFX_MESSAGE_REPLY_BUFFER_NOT_ENOUGH);
            }
        }
        else
        {
            reply_buffer[0] = 0;
        }
    }

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Effect_SetMessageCallback(LFX_Effect* thiz, LFX_EFFECT_MESSAGE_CALLBACK callback)
{
    thiz->message_callback = callback;
    return LFX_SUCCESS;
}

LFX_RESULT LFX_Effect_GetLuaFunction(LFX_Effect* thiz, const char* name)
{
    if (name == NULL)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    lua_State* L = thiz->lua_state;

    int type = lua_getglobal(L, name);
    if (type != LUA_TFUNCTION)
    {
        lua_pop(L, 1);
        RETURN_ERR(LFX_LUA_NO_FUNCTION);
    }

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Effect_CallLuaFunction(LFX_Effect* thiz, int arg_count)
{
    if (arg_count < 0)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    lua_State* L = thiz->lua_state;

    int lua_ret = lua_pcall(L, arg_count, 1, 0);
    if (lua_ret != LUA_OK)
    {
        size_t len = 0;
        const char* err = lua_tolstring(L, -1, &len);
        if (err && len > 0)
        {
            LFX_LOGE(err);
        }
        lua_pop(L, 1);
        RETURN_ERR(LFX_LUA_RUN_ERROR);
    }

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Effect_GetLuaFunctionIntResult(LFX_Effect* thiz)
{
    lua_State* L = thiz->lua_state;

    LFX_RESULT ret = (LFX_RESULT) luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    return ret;
}

LFX_RESULT LFX_Effect_GetThenCallLuaFunction(LFX_Effect* thiz, const char* name)
{
    if (name == NULL)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_RESULT ret = LFX_Effect_GetLuaFunction(thiz, name);
    if (ret != LFX_SUCCESS)
    {
        return ret;
    }

    lua_State* L = thiz->lua_state;

    lua_pushlightuserdata(L, thiz->context);
    lua_pushlightuserdata(L, thiz);
    
    ret = LFX_Effect_CallLuaFunction(thiz, 2);
    if (ret != LFX_SUCCESS)
    {
        return ret;
    }

    ret = LFX_Effect_GetLuaFunctionIntResult(thiz);
    if (ret != LFX_SUCCESS)
    {
        return ret;
    }

    return LFX_SUCCESS;
}

void LFX_Effect_ReadTexture(LFX_Effect* thiz, const LFX_Texture* output_texture, void* output_image)
{
    int buffer_size = output_texture->width * output_texture->height * 4;

    if (thiz->support_pbo)
    {
        int pbo_first_frame = 0;

        if (thiz->pbo_size[0] != output_texture->width || thiz->pbo_size[1] != output_texture->height)
        {
            thiz->pbo_size[0] = output_texture->width;
            thiz->pbo_size[1] = output_texture->height;
            thiz->pbo_index = 0;
            pbo_first_frame = 1;

            for (int i = 0; i < 2; ++i)
            {
                glBindBuffer(GL_PIXEL_PACK_BUFFER, thiz->pbo_read[i]);
                glBufferData(GL_PIXEL_PACK_BUFFER, buffer_size, NULL, GL_STREAM_READ);
            }
        }

        int index = thiz->pbo_index;
        int next_index = (index + 1) % 2;

        glBindBuffer(GL_PIXEL_PACK_BUFFER, thiz->pbo_read[index]);
        glReadPixels(0, 0, output_texture->width, output_texture->height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

        if (pbo_first_frame)
        {
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            glReadPixels(0, 0, output_texture->width, output_texture->height, GL_RGBA, GL_UNSIGNED_BYTE, output_image);
        }
        else
        {
            glBindBuffer(GL_PIXEL_PACK_BUFFER, thiz->pbo_read[next_index]);
            void* mapped_ptr = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, buffer_size, GL_MAP_READ_BIT);
            if (mapped_ptr)
            {
                memcpy(output_image, mapped_ptr, buffer_size);
                glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
            }
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        }

        thiz->pbo_index = (thiz->pbo_index + 1) % 2;
    }
    else
    {
        glReadPixels(0, 0, output_texture->width, output_texture->height, GL_RGBA, GL_UNSIGNED_BYTE, output_image);
    }
}

void LFX_PushTexture(lua_State* L, int index, const LFX_Texture* texture)
{
    const char* keys[] = {
        "id", "target", "format", "width", "height", "filter_mode", "wrap_mode"
    };
    const int values[] = {
        texture->id,
        texture->target,
        texture->format,
        texture->width,
        texture->height,
        texture->filter_mode,
        texture->wrap_mode
    };
    int member_count = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < member_count; ++i)
    {
        lua_pushinteger(L, values[i]);
        lua_setfield(L, index < 0 ? index - 1 : index, keys[i]);
    }
}

void LFX_PopTexture(lua_State* L, int index, LFX_Texture* texture)
{
    const char* keys[] = {
        "id", "target", "format", "width", "height", "filter_mode", "wrap_mode"
    };
    int* pointers[] = {
        &texture->id,
        &texture->target,
        &texture->format,
        &texture->width,
        &texture->height,
        &texture->filter_mode,
        &texture->wrap_mode
    };
    int member_count = sizeof(keys) / sizeof(keys[0]);

    luaL_checktype(L, index, LUA_TTABLE);
    for (int i = 0; i < member_count; ++i)
    {
        int type = lua_getfield(L, index, keys[i]);
        if (type == LUA_TNUMBER)
        {
            *pointers[i] = (int) luaL_checkinteger(L, -1);
        }
        lua_pop(L, 1);
    }
}

const char* LFX_Effect_GetEffectDir(LFX_Effect* thiz)
{
    return thiz->effect_dir;
}

const char* LFX_Effect_MessageCallback(LFX_Effect* thiz, LFX_MESSAGE_ID message_id, const char* message)
{
    if (thiz->message_callback)
    {
        g_message_buffer[0] = 0;
        LFX_RESULT ret = thiz->message_callback(message_id, message, g_message_buffer, MESSAGE_BUFFER_SIZE);
        if (ret != LFX_SUCCESS)
        {
            return NULL;
        }

        return g_message_buffer;
    }
    
    return NULL;
}
