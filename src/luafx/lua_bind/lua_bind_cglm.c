#include "lua_bind_cglm.h"
#include "lua_bind.h"
#include "lualib.h"
#include "lauxlib.h"
#include "cglm/cglm.h"
#include "Context.h"

static int lua_vec3(lua_State* L)
{
    float p1 = (float) lua_tonumber(L, 1);
    float p2 = (float) lua_tonumber(L, 2);
    float p3 = (float) lua_tonumber(L, 3);
    vec3 v = { p1, p2, p3 };
    lua_pushlstring_long(L, (const char*) v, sizeof(v));
    return 1;
}

static int lua_vec4(lua_State* L)
{
    float p1 = (float) lua_tonumber(L, 1);
    float p2 = (float) lua_tonumber(L, 2);
    float p3 = (float) lua_tonumber(L, 3);
    float p4 = (float) lua_tonumber(L, 4);
    vec4 v = { p1, p2, p3, p4 };
    lua_pushlstring_long(L, (const char*) v, sizeof(v));
    return 1;
}

static int lua_mat4(lua_State* L)
{
    mat4 m = GLM_MAT4_IDENTITY_INIT;

    if (lua_istable(L, 1))
    {
        lua_len(L, 1);
        int len = (int) luaL_checkintegerstrict(L, -1);
        lua_pop(L, 1);

        if (len == 16)
        {
            for (int i = 0; i < 16; ++i)
            {
                lua_geti(L, 1, i + 1);
                float v = (float) luaL_checknumber(L, -1);
                lua_pop(L, 1);
                
                m[i / 4][i % 4] = v;
            }
        }
        else
        {
            luaL_argerror(L, 1, "table with 16 numbers or nil expected");
        }
    }

    lua_pushlstring_long(L, (const char*) m, sizeof(m));
    return 1;
}

DEF_FUNC_F_AA(glm_vec3_dot)
DEF_FUNC_F_A(glm_vec3_norm2)
DEF_FUNC_F_A(glm_vec3_norm)
DEF_FUNC_V_AAA(glm_vec3_add)
DEF_FUNC_V_AAA(glm_vec3_sub)
DEF_FUNC_V_A(glm_vec3_normalize)
DEF_FUNC_V_AAA(glm_vec3_cross)
DEF_FUNC_V_AA(glm_translate_make)
DEF_FUNC_V_AA(glm_scale_make)
DEF_FUNC_V_AA(glm_euler_zxy)
DEF_FUNC_V_AAAA(glm_lookat_lh)
DEF_FUNC_V_FFFFA(glm_perspective_lh)
DEF_FUNC_V_FFFFFFA(glm_ortho_lh)
DEF_FUNC_V_AAA(glm_mat4_mul)
DEF_FUNC_V_AAA(glm_mat4_mulv)
DEF_FUNC_V_AAFA(glm_mat4_mulv3)
DEF_FUNC_V_AA(glm_mat4_inv)
DEF_FUNC_V_A(glm_mat4_transpose)
DEF_FUNC_F_F(glm_rad)
DEF_FUNC_F_F(glm_deg)

static const luaL_Reg cglm_funcs[] = {
    REG_FUNC(vec3),
    REG_FUNC(vec4),
    REG_FUNC(mat4),
    REG_FUNC(glm_vec3_dot),
    REG_FUNC(glm_vec3_norm2),
    REG_FUNC(glm_vec3_norm),
    REG_FUNC(glm_vec3_add),
    REG_FUNC(glm_vec3_sub),
    REG_FUNC(glm_vec3_normalize),
    REG_FUNC(glm_vec3_cross),
    REG_FUNC(glm_translate_make),
    REG_FUNC(glm_scale_make),
    REG_FUNC(glm_euler_zxy),
    REG_FUNC(glm_lookat_lh),
    REG_FUNC(glm_perspective_lh),
    REG_FUNC(glm_ortho_lh),
    REG_FUNC(glm_mat4_mul),
    REG_FUNC(glm_mat4_mulv),
    REG_FUNC(glm_mat4_mulv3),
    REG_FUNC(glm_mat4_inv),
    REG_FUNC(glm_mat4_transpose),
    REG_FUNC(glm_rad),
    REG_FUNC(glm_deg),
    { NULL, NULL }
};

void LFX_LuaBindCGLM(lua_State* L)
{
    lua_pushglobaltable(L);
    luaL_setfuncs(L, cglm_funcs, 0);
    
    mat4 LFX_MAT4_FLIP_Y;
    glm_scale_make(LFX_MAT4_FLIP_Y, (vec3) { 1, -1, 1 });
    lua_pushlstring(L, (const char*) LFX_MAT4_FLIP_Y, sizeof(LFX_MAT4_FLIP_Y));
    lua_setfield(L, -2, "LFX_MAT4_FLIP_Y");

    lua_pop(L, 1);
}
