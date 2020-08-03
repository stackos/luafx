local gl = require("gl")

return {
    New = function(self)
        local o = { }
        setmetatable(o, self)
        self.__index = self
        return o
    end,

    Init = function(self, context, font_path, font_size)
        self.context = context
        self.font_path = font_path
        self.font_size = font_size
        self.font_data = nil
        self.font = nil
        self.font_scale = 0
        self.ascent = 0
        self.descend = 0
        self.line_gap = 0

        local pdata = LFX_PointerArrayCreate(1)
        local psize = LFX_BinaryString(4)
        if LFX_Context_LoadFile(context, font_path, pdata, psize) == LFX_SUCCESS then
            LOGI("Font.Init load file: " .. font_path)
            self.font_data = LFX_PointerArrayGetElement(pdata, 0)

            local font_count = stbtt_GetNumberOfFonts(self.font_data)
            if font_count > 0 then
                local font_offset = stbtt_GetFontOffsetForIndex(self.font_data, 0)
                self.font = stbtt_InitFont(self.font_data, font_offset)
            end

            if self.font then
                LOGI("Font.Init init font success")
                self.font_scale = stbtt_ScaleForPixelHeight(self.font, font_size)
                LOGI("Font.Init get scale: " .. self.font_scale)

                local pascend = LFX_BinaryString(4)
                local pdescend = LFX_BinaryString(4)
                local pline_gap = LFX_BinaryString(4)
                stbtt_GetFontVMetrics(self.font, pascend, pdescend, pline_gap)
                self.ascent = string.unpack("i", pascend)
                self.descend = string.unpack("i", pdescend)
                self.line_gap = string.unpack("i", pline_gap)
                LOGI(string.format("Font.Init get font VMetrics, ascent: %d descend: %d line_gap: %d", self.ascent, self.descend, self.line_gap))
            else
                LOGE("Font.Init init font error")
            end
        else
            LOGE("Font.Init load file error: " .. font_path)
        end
        LFX_Free(pdata)
    end,

    Done = function(self)
        if self.font then
            LFX_Free(self.font)
        end
        if self.font_data then
            LFX_Free(self.font_data)
        end
    end,

    Test = function()
        local pdata = LFX_PointerArrayCreate(1)
        local psize = LFX_BinaryString(4)
        if LFX_Context_LoadFile(context, font_path, pdata, psize) == LFX_SUCCESS then
            if font then
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
    end,
}
