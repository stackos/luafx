#include "lua_bind_stbtt.h"
#include "lua_bind.h"
#include "lauxlib.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

static int lua_stbtt_InitFont(lua_State* L)
{
    void* buffer = luaL_checklightuserdata(L, 1);
    int offset = (int) luaL_checkintegerstrict(L, 2);

    stbtt_fontinfo* font = malloc(sizeof(stbtt_fontinfo));
    int ret = stbtt_InitFont(font, buffer, offset);
    if (ret)
    {
        lua_pushlightuserdata(L, font);
    }
    else
    {
        free(font);
        lua_pushnil(L);
    }

    return 1;
}

DEF_FUNC_I_A(stbtt_GetNumberOfFonts)
DEF_FUNC_I_AI(stbtt_GetFontOffsetForIndex)
DEF_FUNC_F_AF(stbtt_ScaleForPixelHeight)
DEF_FUNC_V_AAAA(stbtt_GetFontVMetrics)
DEF_FUNC_I_AI(stbtt_FindGlyphIndex)
DEF_FUNC_V_AIAA(stbtt_GetGlyphHMetrics)
DEF_FUNC_I_AII(stbtt_GetGlyphKernAdvance)
DEF_FUNC_I_AI(stbtt_IsGlyphEmpty)
DEF_FUNC_I_AIAAAA(stbtt_GetGlyphBox)
DEF_FUNC_V_AIFFFFAAAA(stbtt_GetGlyphBitmapBoxSubpixel)
DEF_FUNC_V_AAIIIFFFFI(stbtt_MakeGlyphBitmapSubpixel)

static const luaL_Reg stbtt_funcs[] = {
    REG_FUNC(stbtt_GetNumberOfFonts),
    REG_FUNC(stbtt_GetFontOffsetForIndex),
    REG_FUNC(stbtt_InitFont),
    REG_FUNC(stbtt_ScaleForPixelHeight),
    REG_FUNC(stbtt_GetFontVMetrics),
    REG_FUNC(stbtt_FindGlyphIndex),
    REG_FUNC(stbtt_GetGlyphHMetrics),
    REG_FUNC(stbtt_GetGlyphKernAdvance),
    REG_FUNC(stbtt_IsGlyphEmpty),
    REG_FUNC(stbtt_GetGlyphBox),
    REG_FUNC(stbtt_GetGlyphBitmapBoxSubpixel),
    REG_FUNC(stbtt_MakeGlyphBitmapSubpixel),
    { NULL, NULL }
};

void LFX_LuaBindSTBTT(lua_State* L)
{
    lua_pushglobaltable(L);
    luaL_setfuncs(L, stbtt_funcs, 0);

    lua_pop(L, 1);
}
