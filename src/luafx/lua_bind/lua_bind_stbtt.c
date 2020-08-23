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

static void stbtt_EmboldenBitmap(uint8_t* bitmap, int width, int height, int xstr, int ystr)
{
    if (xstr <= 0 && ystr <= 0)
    {
        return;
    }

    uint8_t* p = bitmap;

    for (int y = 0; y < height ; ++y)
    {
        for (int x = width - 1; x >= 0; --x)
        {
            for (int i = 1; i <= xstr; ++i)
            {
                if (x - i >= 0)
                {
                    if (p[x] + p[x - i] > 255)
                    {
                        p[x] = 255;
                        break;
                    }
                    else
                    {
                        p[x] = p[x] + p[x - i];
                        if (p[x] == 255)
                            break;
                    }
                }
                else
                    break;
            }
        }

        for (int x = 1; x <= ystr; ++x)
        {
            uint8_t* q = p - width * x;
            for (int i = 0; i < width; ++i)
                q[i] |= p[i];
        }

        p += width;
    }
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
DEF_FUNC_V_AIIII(stbtt_EmboldenBitmap)

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
    REG_FUNC(stbtt_EmboldenBitmap),
    { NULL, NULL }
};

void LFX_LuaBindSTBTT(lua_State* L)
{
    lua_pushglobaltable(L);
    luaL_setfuncs(L, stbtt_funcs, 0);

    lua_pop(L, 1);
}
