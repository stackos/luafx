#include "lua_bind_effect.h"
#include "lua_bind.h"
#include "lualib.h"
#include "lauxlib.h"
#include "Effect.h"

DEF_FUNC_S_A(LFX_Effect_GetEffectDir)
DEF_FUNC_S_AIS(LFX_Effect_MessageCallback)

static const luaL_Reg effect_funcs[] = {
    REG_FUNC(LFX_Effect_GetEffectDir),
    REG_FUNC(LFX_Effect_MessageCallback),
    { NULL, NULL }
};

void LFX_LuaBindEffect(lua_State* L)
{
    lua_pushglobaltable(L);
    luaL_setfuncs(L, effect_funcs, 0);

    lua_pop(L, 1);
}
