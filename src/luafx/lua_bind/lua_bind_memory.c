#include "lua_bind_memory.h"
#include "lua_bind.h"
#include "lualib.h"
#include "lauxlib.h"

// aways push long string, avoid string.pack get a cached short string
static int lua_LFX_BinaryString(lua_State* L)
{
    int size = (int) luaL_checkintegerstrict(L, 1);
    if (size > 0)
    {
        void* buffer = malloc(size);
        lua_pushlstring_long(L, buffer, size);
        free(buffer);
    }
    else
    {
        luaL_argerror(L, 1, "size invalid");
    }
    return 1;
}

static int lua_LFX_Malloc(lua_State* L)
{
    int size = (int) luaL_checkintegerstrict(L, 1);
    if (size > 0)
    {
        void* buffer = malloc(size);
        lua_pushlightuserdata(L, buffer);
    }
    else
    {
        luaL_argerror(L, 1, "size invalid");
    }
    return 1;
}

static int lua_LFX_Free(lua_State* L)
{
    void* buffer = luaL_checklightuserdata(L, 1);
    if (buffer)
    {
        free(buffer);
    }
    else
    {
        luaL_argerror(L, 1, "pointer invalid");
    }
    return 0;
}

static int lua_LFX_MemoryAsString(lua_State* L)
{
    const char* string = (const char*) luaL_checklightuserdata(L, 1);
    if (string)
    {
        lua_pushstring(L, string);
    }
    else
    {
        luaL_argerror(L, 1, "pointer invalid");
    }
    return 1;
}

static int lua_LFX_MemoryCopy(lua_State* L)
{
    void* dst = luaL_checklightuserdata(L, 1);
    void* src = luaL_checklightuserdata(L, 2);
    int size = (int) luaL_checkintegerstrict(L, 3);
    if (dst && src && size > 0)
    {
        memcpy(dst, src, size);
    }
    else
    {
        luaL_argerror(L, 1, "invalid input");
    }
    return 0;
}

static int lua_LFX_Int32ArrayCreateFromTable(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    int size = (int) lua_rawlen(L, 1);
    if (size > 0)
    {
        int* array = (int*) malloc(sizeof(int) * size);
        for (int i = 1; i <= size; ++i)
        {
            lua_rawgeti(L, 1, i);
            array[i - 1] = (int) luaL_checkintegerstrict(L, -1);
            lua_pop(L, 1);
        }
        lua_pushlightuserdata(L, array);
    }
    else
    {
        luaL_argerror(L, 1, "size invalid");
    }
    return 1;
}

static int lua_LFX_Uint16ArrayCreateFromTable(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    int size = (int) lua_rawlen(L, 1);
    if (size > 0)
    {
        unsigned short* array = (unsigned short*) malloc(sizeof(unsigned short) * size);
        for (int i = 1; i <= size; ++i)
        {
            lua_rawgeti(L, 1, i);
            array[i - 1] = (unsigned short) luaL_checkintegerstrict(L, -1);
            lua_pop(L, 1);
        }
        lua_pushlightuserdata(L, array);
    }
    else
    {
        luaL_argerror(L, 1, "size invalid");
    }
    return 1;
}

static int lua_LFX_Float32ArrayCreateFromTable(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    int size = (int) lua_rawlen(L, 1);
    if (size > 0)
    {
        float* array = (float*) malloc(sizeof(float) * size);
        for (int i = 1; i <= size; ++i)
        {
            lua_rawgeti(L, 1, i);
            array[i - 1] = (float) luaL_checknumber(L, -1);
            lua_pop(L, 1);
        }
        lua_pushlightuserdata(L, array);
    }
    else
    {
        luaL_argerror(L, 1, "size invalid");
    }
    return 1;
}

static int lua_LFX_PointerArrayCreate(lua_State* L)
{
    int size = (int) luaL_checkinteger(L, 1);
    if (size > 0)
    {
        void** array = (void**) malloc(sizeof(void*) * size);
        for (int i = 1; i <= size; ++i)
        {
            array[i - 1] = NULL;
        }
        lua_pushlightuserdata(L, array);
    }
    else
    {
        luaL_argerror(L, 1, "size invalid");
    }
    return 1;
}

static int lua_LFX_Int32ArrayGetElement(lua_State* L)
{
    int* array = (int*) lua_touserdata_or_string(L, 1);
    int index = (int) luaL_checkintegerstrict(L, 2);
    if (array)
    {
        if (index >= 0)
        {
            lua_pushinteger(L, (lua_Integer) array[index]);
        }
        else
        {
            luaL_argerror(L, 1, "index invalid");
        }
    }
    else
    {
        luaL_argerror(L, 1, "pointer invalid");
    }
    return 1;
}

static int lua_LFX_Int32ArraySetElement(lua_State* L)
{
    int* array = (int*) lua_touserdata_or_string(L, 1);
    int index = (int) luaL_checkintegerstrict(L, 2);
    int value = (int) luaL_checkintegerstrict(L, 3);
    if (array)
    {
        if (index >= 0)
        {
            array[index] = value;
        }
        else
        {
            luaL_argerror(L, 1, "index invalid");
        }
    }
    else
    {
        luaL_argerror(L, 1, "pointer invalid");
    }
    return 0;
}

static int lua_LFX_Uint16ArrayGetElement(lua_State* L)
{
    uint16_t* array = (uint16_t*) lua_touserdata_or_string(L, 1);
    int index = (int) luaL_checkintegerstrict(L, 2);
    if (array)
    {
        if (index >= 0)
        {
            lua_pushinteger(L, (lua_Integer) array[index]);
        }
        else
        {
            luaL_argerror(L, 1, "index invalid");
        }
    }
    else
    {
        luaL_argerror(L, 1, "pointer invalid");
    }
    return 1;
}

static int lua_LFX_Uint16ArraySetElement(lua_State* L)
{
    uint16_t* array = (uint16_t*) lua_touserdata_or_string(L, 1);
    int index = (int) luaL_checkintegerstrict(L, 2);
    uint16_t value = (uint16_t) luaL_checkintegerstrict(L, 3);
    if (array)
    {
        if (index >= 0)
        {
            array[index] = value;
        }
        else
        {
            luaL_argerror(L, 1, "index invalid");
        }
    }
    else
    {
        luaL_argerror(L, 1, "pointer invalid");
    }
    return 0;
}

static int lua_LFX_Float32ArrayGetElement(lua_State* L)
{
    float* array = (float*) lua_touserdata_or_string(L, 1);
    int index = (int) luaL_checkintegerstrict(L, 2);
    if (array)
    {
        if (index >= 0)
        {
            lua_pushnumber(L, (lua_Number) array[index]);
        }
        else
        {
            luaL_argerror(L, 1, "index invalid");
        }
    }
    else
    {
        luaL_argerror(L, 1, "pointer invalid");
    }
    return 1;
}

static int lua_LFX_Float32ArraySetElement(lua_State* L)
{
    float* array = (float*) lua_touserdata_or_string(L, 1);
    int index = (int) luaL_checkintegerstrict(L, 2);
    float value = (float) luaL_checknumber(L, 3);
    if (array)
    {
        if (index >= 0)
        {
            array[index] = value;
        }
        else
        {
            luaL_argerror(L, 1, "index invalid");
        }
    }
    else
    {
        luaL_argerror(L, 1, "pointer invalid");
    }
    return 0;
}

static int lua_LFX_PointerArrayGetElement(lua_State* L)
{
    void** array = (void**) luaL_checklightuserdata(L, 1);
    int index = (int) luaL_checkintegerstrict(L, 2);
    if (array)
    {
        if (index >= 0)
        {
            lua_pushlightuserdata(L, array[index]);
        }
        else
        {
            luaL_argerror(L, 1, "index invalid");
        }
    }
    else
    {
        luaL_argerror(L, 1, "pointer invalid");
    }
    return 1;
}

static int lua_LFX_PointerArraySetElement(lua_State* L)
{
    void** array = (void**) luaL_checklightuserdata(L, 1);
    int index = (int) luaL_checkintegerstrict(L, 2);
    void* value = lua_touserdata_or_string(L, 3);
    if (array)
    {
        if (index >= 0)
        {
            array[index] = value;
        }
        else
        {
            luaL_argerror(L, 1, "index invalid");
        }
    }
    else
    {
        luaL_argerror(L, 1, "pointer invalid");
    }
    return 0;
}

static const luaL_Reg memory_funcs[] = {
    REG_FUNC(LFX_BinaryString),
    REG_FUNC(LFX_Malloc),
    REG_FUNC(LFX_Free),
    REG_FUNC(LFX_MemoryAsString),
    REG_FUNC(LFX_MemoryCopy),
    REG_FUNC(LFX_Int32ArrayCreateFromTable),
    REG_FUNC(LFX_Uint16ArrayCreateFromTable),
    REG_FUNC(LFX_Float32ArrayCreateFromTable),
    REG_FUNC(LFX_PointerArrayCreate),
    REG_FUNC(LFX_Int32ArrayGetElement),
    REG_FUNC(LFX_Int32ArraySetElement),
    REG_FUNC(LFX_Uint16ArrayGetElement),
    REG_FUNC(LFX_Uint16ArraySetElement),
    REG_FUNC(LFX_Float32ArrayGetElement),
    REG_FUNC(LFX_Float32ArraySetElement),
    REG_FUNC(LFX_PointerArrayGetElement),
    REG_FUNC(LFX_PointerArraySetElement),
    { NULL, NULL }
};

void LFX_LuaBindMemory(lua_State* L)
{
    lua_pushglobaltable(L);
    luaL_setfuncs(L, memory_funcs, 0);

    lua_pop(L, 1);
}
