#pragma once

#include "luafx.h"
#include "lua.h"

#define DEF_FUNC_V_V(name) static int lua_##name(lua_State* L) { \
    name(); \
    return 0; }

#define DEF_FUNC_V_I(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    name(p1); \
    return 0; }

#define DEF_FUNC_V_F(name) static int lua_##name(lua_State* L) { \
    float p1 = (float) luaL_checknumber(L, 1); \
    name(p1); \
    return 0; }

#define DEF_FUNC_V_S(name) static int lua_##name(lua_State* L) { \
    const char* p1 = luaL_checkstringstrict(L, 1); \
    name(p1); \
    return 0; }

#define DEF_FUNC_V_A(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    name(p1); \
    return 0; }

#define DEF_FUNC_I_I(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int r = name(p1); \
    lua_pushinteger(L, (lua_Integer) r); \
    return 1; }

#define DEF_FUNC_I_V(name) static int lua_##name(lua_State* L) { \
    int r = name(); \
    lua_pushinteger(L, (lua_Integer) r); \
    return 1; }

#define DEF_FUNC_S_I(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    const char* r = (const char*) name(p1); \
    lua_pushstring(L, r); \
    return 1; }

#define DEF_FUNC_S_A(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    const char* r = (const char*) name(p1); \
    lua_pushstring(L, r); \
    return 1; }

#define DEF_FUNC_F_A(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    float r = name(p1); \
    lua_pushnumber(L, (lua_Number) r); \
    return 1; }

#define DEF_FUNC_F_F(name) static int lua_##name(lua_State* L) { \
    float p1 = (float) luaL_checknumber(L, 1); \
    float r = name(p1); \
    lua_pushnumber(L, (lua_Number) r); \
    return 1; }

#define DEF_FUNC_V_II(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    name(p1, p2); \
    return 0; }

#define DEF_FUNC_V_IF(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    float p2 = (float) luaL_checknumber(L, 2); \
    name(p1, p2); \
    return 0; }

#define DEF_FUNC_V_FF(name) static int lua_##name(lua_State* L) { \
    float p1 = (float) luaL_checknumber(L, 1); \
    float p2 = (float) luaL_checknumber(L, 2); \
    name(p1, p2); \
    return 0; }

#define DEF_FUNC_V_FI(name) static int lua_##name(lua_State* L) { \
    float p1 = (float) luaL_checknumber(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    name(p1, p2); \
    return 0; }

#define DEF_FUNC_V_IA(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    void* p2 = lua_touserdata_or_string(L, 2); \
    name(p1, p2); \
    return 0; }

#define DEF_FUNC_V_AA(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    void* p2 = lua_touserdata_or_string(L, 2); \
    name(p1, p2); \
    return 0; }

#define DEF_FUNC_F_AA(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    void* p2 = lua_touserdata_or_string(L, 2); \
    float r = name(p1, p2); \
    lua_pushnumber(L, (lua_Number) r); \
    return 1; }

#define DEF_FUNC_I_IS(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    const char* p2 = luaL_checkstringstrict(L, 2); \
    int r = name(p1, p2); \
    lua_pushinteger(L, (lua_Integer) r); \
    return 1; }

#define DEF_FUNC_I_AS(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    const char* p2 = luaL_checkstringstrict(L, 2); \
    int r = name(p1, p2); \
    lua_pushinteger(L, (lua_Integer) r); \
    return 1; }

#define DEF_FUNC_V_IIS(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    const char* p3 = luaL_checkstringstrict(L, 3); \
    name(p1, p2, p3); \
    return 0; }

#define DEF_FUNC_V_III(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    int p3 = (int) luaL_checkintegerstrict(L, 3); \
    name(p1, p2, p3); \
    return 0; }

#define DEF_FUNC_V_IIF(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    float p3 = (float) luaL_checknumber(L, 3); \
    name(p1, p2, p3); \
    return 0; }

#define DEF_FUNC_V_IFF(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    float p2 = (float) luaL_checknumber(L, 2); \
    float p3 = (float) luaL_checknumber(L, 3); \
    name(p1, p2, p3); \
    return 0; }

#define DEF_FUNC_V_IIA(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    void* p3 = lua_touserdata_or_string(L, 3); \
    name(p1, p2, p3); \
    return 0; }

#define DEF_FUNC_V_AAA(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    void* p2 = lua_touserdata_or_string(L, 2); \
    void* p3 = lua_touserdata_or_string(L, 3); \
    name(p1, p2, p3); \
    return 0; }

#define DEF_FUNC_I_ASA(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    const char* p2 = luaL_checkstringstrict(L, 2); \
    void* p3 = lua_touserdata_or_string(L, 3); \
    int r = name(p1, p2, p3); \
    lua_pushinteger(L, (lua_Integer) r); \
    return 1; }

#define DEF_FUNC_S_AIS(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    const char* p3 = luaL_checkstringstrict(L, 3); \
    const char* r = (const char*) name(p1, p2, p3); \
    lua_pushstring(L, r); \
    return 1; }

#define DEF_FUNC_V_IIII(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    int p3 = (int) luaL_checkintegerstrict(L, 3); \
    int p4 = (int) luaL_checkintegerstrict(L, 4); \
    name(p1, p2, p3, p4); \
    return 0; }

#define DEF_FUNC_V_IIAI(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    void* p3 = lua_touserdata_or_string(L, 3); \
    int p4 = (int) luaL_checkintegerstrict(L, 4); \
    name(p1, p2, p3, p4); \
    return 0; }

#define GET_INT_OR_ARRAY(p, index) \
    void* p = NULL; \
    if (lua_isinteger(L, index)) \
    { \
        p = (void*) (size_t) lua_tointeger(L, index); \
    } \
    else \
    { \
        p = lua_touserdata_or_string(L, index); \
    } \

#define DEF_FUNC_V_IIIA(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    int p3 = (int) luaL_checkintegerstrict(L, 3); \
    GET_INT_OR_ARRAY(p4, 4); \
    name(p1, p2, p3, p4); \
    return 0; }

#define DEF_FUNC_V_IIAA(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    void* p3 = lua_touserdata_or_string(L, 3); \
    void* p4 = lua_touserdata_or_string(L, 4); \
    name(p1, p2, p3, p4); \
    return 0; }

#define DEF_FUNC_V_IFFF(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    float p2 = (float) luaL_checknumber(L, 2); \
    float p3 = (float) luaL_checknumber(L, 3); \
    float p4 = (float) luaL_checknumber(L, 4); \
    name(p1, p2, p3, p4); \
    return 0; }

#define DEF_FUNC_V_FFFF(name) static int lua_##name(lua_State* L) { \
    float p1 = (float) luaL_checknumber(L, 1); \
    float p2 = (float) luaL_checknumber(L, 2); \
    float p3 = (float) luaL_checknumber(L, 3); \
    float p4 = (float) luaL_checknumber(L, 4); \
    name(p1, p2, p3, p4); \
    return 0; }

#define DEF_FUNC_V_AAFA(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    void* p2 = lua_touserdata_or_string(L, 2); \
    float p3 = (float) luaL_checknumber(L, 3); \
    void* p4 = lua_touserdata_or_string(L, 4); \
    name(p1, p2, p3, p4); \
    return 0; }

#define DEF_FUNC_V_AAAA(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    void* p2 = lua_touserdata_or_string(L, 2); \
    void* p3 = lua_touserdata_or_string(L, 3); \
    void* p4 = lua_touserdata_or_string(L, 4); \
    name(p1, p2, p3, p4); \
    return 0; }

#define DEF_FUNC_I_ASSA(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    const char* p2 = luaL_checkstringstrict(L, 2); \
    const char* p3 = luaL_checkstringstrict(L, 3); \
    void* p4 = lua_touserdata_or_string(L, 4); \
    int r = name(p1, p2, p3, p4); \
    lua_pushinteger(L, (lua_Integer) r); \
    return 1; }

#define DEF_FUNC_I_ASAA(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    const char* p2 = luaL_checkstringstrict(L, 2); \
    void* p3 = lua_touserdata_or_string(L, 3); \
    void* p4 = lua_touserdata_or_string(L, 4); \
    int r = name(p1, p2, p3, p4); \
    lua_pushinteger(L, (lua_Integer) r); \
    return 1; }

#define DEF_FUNC_I_AAAA(name) static int lua_##name(lua_State* L) { \
    void* p1 = lua_touserdata_or_string(L, 1); \
    void* p2 = lua_touserdata_or_string(L, 2); \
    void* p3 = lua_touserdata_or_string(L, 3); \
    void* p4 = lua_touserdata_or_string(L, 4); \
    int r = name(p1, p2, p3, p4); \
    lua_pushinteger(L, (lua_Integer) r); \
    return 1; }

#define DEF_FUNC_A_IIII(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    int p3 = (int) luaL_checkintegerstrict(L, 3); \
    int p4 = (int) luaL_checkintegerstrict(L, 4); \
    void* r = name(p1, p2, p3, p4); \
    lua_pushlightuserdata(L, r); \
    return 1; }

#define DEF_FUNC_V_IIIII(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    int p3 = (int) luaL_checkintegerstrict(L, 3); \
    int p4 = (int) luaL_checkintegerstrict(L, 4); \
    int p5 = (int) luaL_checkintegerstrict(L, 5); \
    name(p1, p2, p3, p4, p5); \
    return 0; }

#define DEF_FUNC_V_IFFFF(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    float p2 = (float) luaL_checknumber(L, 2); \
    float p3 = (float) luaL_checknumber(L, 3); \
    float p4 = (float) luaL_checknumber(L, 4); \
    float p5 = (float) luaL_checknumber(L, 5); \
    name(p1, p2, p3, p4, p5); \
    return 0; }

#define DEF_FUNC_V_FFFFA(name) static int lua_##name(lua_State* L) { \
    float p1 = (float) luaL_checknumber(L, 1); \
    float p2 = (float) luaL_checknumber(L, 2); \
    float p3 = (float) luaL_checknumber(L, 3); \
    float p4 = (float) luaL_checknumber(L, 4); \
    void* p5 = lua_touserdata_or_string(L, 5); \
    name(p1, p2, p3, p4, p5); \
    return 0; }

#define DEF_FUNC_V_IAIAI(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    void* p2 = lua_touserdata_or_string(L, 2); \
    int p3 = (int) luaL_checkintegerstrict(L, 3); \
    void* p4 = lua_touserdata_or_string(L, 4); \
    int p5 = (int) luaL_checkintegerstrict(L, 5); \
    name(p1, p2, p3, p4, p5); \
    return 0; }

#define DEF_FUNC_V_IIIAI(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    int p3 = (int) luaL_checkintegerstrict(L, 3); \
    GET_INT_OR_ARRAY(p4, 4); \
    int p5 = (int) luaL_checkintegerstrict(L, 5); \
    name(p1, p2, p3, p4, p5); \
    return 0; }

#define DEF_FUNC_V_IIIIIA(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    int p3 = (int) luaL_checkintegerstrict(L, 3); \
    int p4 = (int) luaL_checkintegerstrict(L, 4); \
    int p5 = (int) luaL_checkintegerstrict(L, 5); \
    GET_INT_OR_ARRAY(p6, 6); \
    name(p1, p2, p3, p4, p5, p6); \
    return 0; }

#define DEF_FUNC_V_IIIIIIA(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    int p3 = (int) luaL_checkintegerstrict(L, 3); \
    int p4 = (int) luaL_checkintegerstrict(L, 4); \
    int p5 = (int) luaL_checkintegerstrict(L, 5); \
    int p6 = (int) luaL_checkintegerstrict(L, 6); \
    GET_INT_OR_ARRAY(p7, 7); \
    name(p1, p2, p3, p4, p5, p6, p7); \
    return 0; }

#define DEF_FUNC_V_FFFFFFA(name) static int lua_##name(lua_State* L) { \
    float p1 = (float) luaL_checknumber(L, 1); \
    float p2 = (float) luaL_checknumber(L, 2); \
    float p3 = (float) luaL_checknumber(L, 3); \
    float p4 = (float) luaL_checknumber(L, 4); \
    float p5 = (float) luaL_checknumber(L, 5); \
    float p6 = (float) luaL_checknumber(L, 6); \
    void* p7 = lua_touserdata_or_string(L, 7); \
    name(p1, p2, p3, p4, p5, p6, p7); \
    return 0; }

#define DEF_FUNC_V_IIIAAAA(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    int p3 = (int) luaL_checkintegerstrict(L, 3); \
    void* p4 = lua_touserdata_or_string(L, 4); \
    void* p5 = lua_touserdata_or_string(L, 5); \
    void* p6 = lua_touserdata_or_string(L, 6); \
    void* p7 = lua_touserdata_or_string(L, 7); \
    name(p1, p2, p3, p4, p5, p6, p7); \
    return 0; }

#define DEF_FUNC_V_IIIIIIII(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    int p3 = (int) luaL_checkintegerstrict(L, 3); \
    int p4 = (int) luaL_checkintegerstrict(L, 4); \
    int p5 = (int) luaL_checkintegerstrict(L, 5); \
    int p6 = (int) luaL_checkintegerstrict(L, 6); \
    int p7 = (int) luaL_checkintegerstrict(L, 7); \
    int p8 = (int) luaL_checkintegerstrict(L, 8); \
    name(p1, p2, p3, p4, p5, p6, p7, p8); \
    return 0; }

#define DEF_FUNC_V_IIIIIIIA(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    int p3 = (int) luaL_checkintegerstrict(L, 3); \
    int p4 = (int) luaL_checkintegerstrict(L, 4); \
    int p5 = (int) luaL_checkintegerstrict(L, 5); \
    int p6 = (int) luaL_checkintegerstrict(L, 6); \
    int p7 = (int) luaL_checkintegerstrict(L, 7); \
    void* p8 = lua_touserdata_or_string(L, 8); \
    name(p1, p2, p3, p4, p5, p6, p7, p8); \
    return 0; }

#define DEF_FUNC_V_IIIIIIIIA(name) static int lua_##name(lua_State* L) { \
    int p1 = (int) luaL_checkintegerstrict(L, 1); \
    int p2 = (int) luaL_checkintegerstrict(L, 2); \
    int p3 = (int) luaL_checkintegerstrict(L, 3); \
    int p4 = (int) luaL_checkintegerstrict(L, 4); \
    int p5 = (int) luaL_checkintegerstrict(L, 5); \
    int p6 = (int) luaL_checkintegerstrict(L, 6); \
    int p7 = (int) luaL_checkintegerstrict(L, 7); \
    int p8 = (int) luaL_checkintegerstrict(L, 8); \
    void* p9 = lua_touserdata_or_string(L, 9); \
    name(p1, p2, p3, p4, p5, p6, p7, p8, p9); \
    return 0; }

#define REG_FUNC(name) { #name, lua_##name }

typedef struct DefineReg
{
    const char* name;
    int value;
} DefineReg;

#define REG_DEF(name) { #name, name }

void* lua_touserdata_or_string(lua_State* L, int index);
void* luaL_checklightuserdata(lua_State* L, int index);
const char* luaL_checkstringstrict(lua_State* L, int index);
lua_Integer luaL_checkintegerstrict(lua_State* L, int index);
const char* lua_pushlstring_long(lua_State* L, const char* s, size_t size);
void luaL_setintfields(lua_State* L, const DefineReg* fields, int count);
