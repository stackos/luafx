#include "lua_bind_cjson.h"
#include "lua_bind.h"
#include "lualib.h"
#include "lauxlib.h"
#include "cJSON.h"
#include "Context.h"

static void PushJsonValue(lua_State* L, const cJSON* node);

static void ParseJsonToTable(lua_State* L, const cJSON* node)
{
    if (cJSON_IsArray(node))
    {
        int size = cJSON_GetArraySize(node);
        for (int i = 0; i < size; ++i)
        {
            cJSON* child = cJSON_GetArrayItem(node, i);
            if (child)
            {
                int index = i + 1;
                PushJsonValue(L, child);
                lua_rawseti(L, -2, index);
            }
            else
            {
                lua_pushnil(L);
            }
        }
    }
    else if (cJSON_IsObject(node))
    {
        const cJSON* child = NULL;
        cJSON_ArrayForEach(child, node)
        {
            if (child && child->string)
            {
                const char* key = child->string;
                PushJsonValue(L, child);
                lua_setfield(L, -2, key);
            }
        }
    }
}

static void PushJsonValue(lua_State* L, const cJSON* node)
{
    if (cJSON_IsArray(node) || cJSON_IsObject(node))
    {
        lua_newtable(L);
        ParseJsonToTable(L, node);
    }
    else if (cJSON_IsTrue(node))
    {
        lua_pushboolean(L, 1);
    }
    else if (cJSON_IsFalse(node))
    {
        lua_pushboolean(L, 0);
    }
    else if (cJSON_IsNumber(node))
    {
        lua_pushnumber(L, node->valuedouble);
    }
    else if (cJSON_IsString(node) && node->valuestring)
    {
        lua_pushstring(L, node->valuestring);
    }
    else
    {
        lua_pushnil(L);
    }
}

static int lua_LFX_JsonParse(lua_State* L)
{
    const char* str = luaL_checkstringstrict(L, 1);
    if (str)
    {
        cJSON* json = cJSON_Parse(str);
        if (json)
        {
            PushJsonValue(L, json);
            cJSON_Delete(json);
        }
        else
        {
            LFX_LOGE("lua_LFX_JsonParse fail");
            lua_pushnil(L);
        }
    }
    else
    {
        luaL_argerror(L, 1, "string expected");
    }

    return 1;
}

static void PopJsonValue(lua_State* L, cJSON** node)
{
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while (lua_next(L, -2))
        {
            cJSON* child = NULL;
            PopJsonValue(L, &child);

            int key_type = lua_type(L, -2);
            if (key_type == LUA_TNUMBER)
            {
                if (*node == NULL)
                {
                    *node = cJSON_CreateArray();
                }

                cJSON_AddItemToArray(*node, child);
            }
            else if (key_type == LUA_TSTRING)
            {
                if (*node == NULL)
                {
                    *node = cJSON_CreateObject();
                }

                const char* key = lua_tostring(L, -2);
                cJSON_AddItemToObject(*node, key, child);
            }

            lua_pop(L, 1);
        }

        // empty array
        if (*node == NULL)
        {
            *node = cJSON_CreateArray();
        }
    }
    else if (lua_isboolean(L, -1))
    {
        *node = cJSON_CreateBool(lua_toboolean(L, -1));
    }
    else if (lua_type(L, -1) == LUA_TNUMBER)
    {
        *node = cJSON_CreateNumber(lua_tonumber(L, -1));
    }
    else if (lua_type(L, -1) == LUA_TSTRING)
    {
        *node = cJSON_CreateString(lua_tostring(L, -1));
    }
    else
    {
        *node = cJSON_CreateNull();
    }
}

static int lua_LFX_JsonPrint(lua_State* L)
{
    cJSON* root = NULL;
    PopJsonValue(L, &root);
    char* json = cJSON_Print(root);
    if (json)
    {
        lua_pushstring(L, json);
        free(json);
    }
    else
    {
        LFX_LOGE("lua_LFX_JsonPrint fail");
        lua_pushnil(L);
    }
    cJSON_Delete(root);

    return 1;
}

static const luaL_Reg cjson_funcs[] = {
    REG_FUNC(LFX_JsonParse),
    REG_FUNC(LFX_JsonPrint),
    { NULL, NULL }
};

void LFX_LuaBindCJSON(lua_State* L)
{
    lua_pushglobaltable(L);
    luaL_setfuncs(L, cjson_funcs, 0);

    lua_pop(L, 1);
}
