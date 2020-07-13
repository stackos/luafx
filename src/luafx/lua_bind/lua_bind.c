#include "lua_bind.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lstring.h"
#include "lapi.h"

void* lua_touserdata_or_string(lua_State* L, int index)
{
    if (lua_islightuserdata(L, index))
    {
        return lua_touserdata(L, index);
    }
    else if (lua_type(L, index) == LUA_TSTRING)
    {
        return (void*) lua_tostring(L, index);
    }
    else if (lua_isnoneornil(L, index))
    {
        return NULL;
    }
    else
    {
        luaL_argerror(L, index, "lightuserdata, binary string or nil expected");
    }
    return NULL;
}

void* luaL_checklightuserdata(lua_State* L, int index)
{
    if (lua_islightuserdata(L, index))
    {
        return lua_touserdata(L, index);
    }
    else
    {
        luaL_argerror(L, index, "lightuserdata expected");
    }
    return NULL;
}

const char* luaL_checkstringstrict(lua_State* L, int index)
{
    if (lua_type(L, index) == LUA_TSTRING)
    {
        return lua_tostring(L, index);
    }
    else
    {
        luaL_argerror(L, index, "string expected");
    }
    return NULL;
}

lua_Integer luaL_checkintegerstrict(lua_State* L, int index)
{
    if (lua_isinteger(L, index))
    {
        return lua_tointeger(L, index);
    }
    else
    {
        luaL_argerror(L, index, "integer expected");
    }
    return 0;
}

const char* lua_pushlstring_long(lua_State* L, const char* s, size_t size)
{
    if (size == 0)
    {
        luaL_error(L, "size invalid");
    }
    else if (size >= (MAX_SIZE - sizeof(TString)))
    {
        luaM_toobig(L);
    }

    TString* ts;
    lua_lock(L);
    ts = luaS_createlngstrobj(L, size);
    memcpy(getstr(ts), s, size);
    setsvalue2s(L, L->top, ts);
    api_incr_top(L);
    luaC_checkGC(L);
    lua_unlock(L);
    return getstr(ts);
}

void luaL_setintfields(lua_State* L, const DefineReg* fields, int count)
{
    for (int i = 0; i < count; ++i)
    {
        lua_pushinteger(L, fields[i].value);
        lua_setfield(L, -2, fields[i].name);
    }
}
