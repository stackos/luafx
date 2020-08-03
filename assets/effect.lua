local gl = require("gl")

local demo = require("demo/demo_triangle")
--local demo = require("demo/demo_cube")
--local demo = require("demo/demo_depth_texture")
--local demo = require("demo/demo_instance")

local BuildPlatformNames = {
    "Windows",
    "Android",
    "Mac",
    "iOS",
    "WebAssembly"
}

local _effect = {
    timestamp = 0,
    shared_quad_renderer = require("QuadRenderer"):New(),

    Init = function(self, context, effect)
        local build_platform = LFX_BinaryString(4)
        LFX_Context_GetBuildPlatform(context, build_platform)
        local platform = string.unpack("i", build_platform)

        LOGD(string.format("BuildPlatform: %s", BuildPlatformNames[platform + 1]))

        self.shared_quad_renderer:Init(context, effect)

        demo.Init(context, effect)
    end,

    Done = function(self, context, effect)
        self.shared_quad_renderer:Done(context, effect)

        demo.Done(context, effect)
    end,

    Render = function(self, context, effect, input_texture, output_texture)
        gl.AttachColorTexture(output_texture)
        LFX_Context_RenderQuad(context, input_texture, LFX_MAT4_FLIP_Y)

        demo.Render(context, effect, input_texture, output_texture)
    end,

    SetTimestamp = function(self, context, effect, message)
        local msg = LFX_JsonParse(message)
        self.timestamp = math.floor(msg.timestamp)
    end,
}

function Effect_Load(context, effect)
    LOGD("Effect_Load")
    _effect:Init(context, effect)

    -- test stb truetype font
    local build_platform = LFX_BinaryString(4)
    LFX_Context_GetBuildPlatform(context, build_platform)
    local platform = string.unpack("i", build_platform)
    local font_path = "c:/windows/fonts/arialbd.ttf"
    if platform == LFX_BUILD_PLATFORM_MAC then
        font_path = "/System/Library/Fonts/PingFang.ttc"
    end
    
    local pdata = LFX_PointerArrayCreate(1)
    local psize = LFX_BinaryString(4)
    if LFX_Context_LoadFile(context, font_path, pdata, psize) == LFX_SUCCESS then
        LOGD("LFX_Context_LoadFile " .. string.unpack("i", psize))
        local data = LFX_PointerArrayGetElement(pdata, 0)
        
        local font_count = stbtt_GetNumberOfFonts(data)
        LOGD("stbtt_GetNumberOfFonts " .. font_count)
        
        local font = nil
        if font_count > 0 then
            local font_offset = stbtt_GetFontOffsetForIndex(data, 0)
            font = stbtt_InitFont(data, font_offset)
        end

        if font then
            LOGD("stbtt_InitFont")

            local scale = stbtt_ScaleForPixelHeight(font, 15)
            LOGD("stbtt_ScaleForPixelHeight " .. scale)

            local pascend = LFX_BinaryString(4)
            local pdescend = LFX_BinaryString(4)
            local pline_gap = LFX_BinaryString(4)
            stbtt_GetFontVMetrics(font, pascend, pdescend, pline_gap)
            LOGD(string.format("stbtt_GetFontVMetrics %d %d %d",
                string.unpack("i", pascend),
                string.unpack("i", pdescend),
                string.unpack("i", pline_gap)))
            
            local ascent = string.unpack("i", pascend)
            local baseline = ascent * scale
            LOGD("baseline " .. baseline)

            local xpos = 2

            local str = "Hello World"
            local codes = { }
            for pos, code in utf8.codes(str) do
                codes[#codes + 1] = code
            end
            for i = 1, #codes do
                local code = codes[i]
                LOGD("code " .. code)
                local glyph = stbtt_FindGlyphIndex(font, code)
                LOGD("glyph " .. glyph)

                local padvance = LFX_BinaryString(4)
                local pleft_bearing = LFX_BinaryString(4)
                stbtt_GetGlyphHMetrics(font, glyph, padvance, pleft_bearing)
                local advance = string.unpack("i", padvance)
                local left_bearing = string.unpack("i", pleft_bearing)
                LOGD("stbtt_GetGlyphHMetrics advance " .. advance .. " left_bearing " .. left_bearing)

                if stbtt_IsGlyphEmpty(font, glyph) == 0 then
                    local px0 = LFX_BinaryString(4)
                    local py0 = LFX_BinaryString(4)
                    local px1 = LFX_BinaryString(4)
                    local py1 = LFX_BinaryString(4)
                    if stbtt_GetGlyphBox(font, glyph, px0, py0, px1, py1) == 1 then
                        LOGD(string.format("stbtt_GetGlyphBox %d %d %d %d",
                            string.unpack("i", px0),
                            string.unpack("i", py0),
                            string.unpack("i", px1),
                            string.unpack("i", py1)))
                    end

                    local x_shift = xpos - math.floor(xpos)
                    stbtt_GetGlyphBitmapBoxSubpixel(font, glyph, scale, scale, x_shift, 0, px0, py0, px1, py1)
                    local x0 = string.unpack("i", px0)
                    local y0 = string.unpack("i", py0)
                    local x1 = string.unpack("i", px1)
                    local y1 = string.unpack("i", py1)
                    LOGD(string.format("stbtt_GetGlyphBitmapBoxSubpixel %d %d %d %d", x0, y0, x1, y1))

                    local w = x1 - x0
                    local h = y1 - y0
                    local pixels = LFX_Malloc(w * h)
                    stbtt_MakeGlyphBitmapSubpixel(font, pixels, w, h, w, scale, scale, x_shift, 0, glyph)
                    LFX_Free(pixels)
                end

                LOGD("xpos " .. xpos)

                xpos = xpos + advance * scale

                if i < #codes then
                    local kern = stbtt_GetGlyphKernAdvance(font, glyph, stbtt_FindGlyphIndex(font, codes[i + 1]))
                    LOGD("kern " .. kern)

                    xpos = xpos + kern * scale
                end
            end

            LFX_Free(font)
        end
        LFX_Free(pdata)
    end

    return LFX_SUCCESS
end

function Effect_Done(context, effect)
    LOGD("Effect_Done")
    _effect:Done(context, effect)
    return LFX_SUCCESS
end

function Effect_Render(context, effect, input_texture, output_texture)
    _effect:Render(context, effect, input_texture, output_texture)
    return LFX_SUCCESS
end

function Effect_SendMessage(context, effect, message_id, message)
    if message_id == LFX_MESSAGE_ID_SET_EFFECT_TIMESTAMP then
        _effect:SetTimestamp(context, effect, message)
    end
end
