local gl = require("gl")

local Font = { }

-- public
function Font.New()
    local o = { }
    setmetatable(o, Font)
    Font.__index = Font
    return o
end

function Font:Init(context, font_path, font_size)
    self.context = context
    self.font_path = font_path
    self.font_size = font_size
    self.font_data = nil
    self.font = nil
    self.font_scale = 0
    self.ascent = 0
    self.descend = 0
    self.line_gap = 0
    self.glyph_map = { }
    self.texture = nil
    self.texture_format = gl.GetRedTextureFormat(context)
    self.texture_size = { 1024, 1024 }
    self.texture_pos = { 0, 0 }
    self.texture_pos_y_max = 0

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
end

function Font:Done()
    if self.texture then
        gl.DeleteTexture(self.texture)
    end
    if self.font then
        LFX_Free(self.font)
    end
    if self.font_data then
        LFX_Free(self.font_data)
    end
end

function Font:GetGlyph(code)
    if self.font == nil then
        return nil
    end

    local char = utf8.char(code)
    local glyph = self.glyph_map[char]
    if glyph then
        return glyph
    end

    glyph = {
        char = char, -- string char
        code = code, -- int code
        glyph_index = 0, -- not exist
        advance = 0,
        left_bearing = 0,
        empty = true, -- no bitmap
        box = { 0, 0, 0, 0 },
        bitmap_box = { 0, 0, 0, 0 },
        texture_rect = { 0, 0, 0, 0 },
    }
    self.glyph_map[char] = glyph

    glyph.glyph_index = stbtt_FindGlyphIndex(self.font, glyph.code)
    if glyph.glyph_index == 0 then
        return glyph
    end

    local padvance = LFX_BinaryString(4)
    local pleft_bearing = LFX_BinaryString(4)
    stbtt_GetGlyphHMetrics(self.font, glyph.glyph_index, padvance, pleft_bearing)
    glyph.advance = string.unpack("i", padvance)
    glyph.left_bearing = string.unpack("i", pleft_bearing)
    glyph.empty = stbtt_IsGlyphEmpty(self.font, glyph.glyph_index) ~= 0

    if glyph.empty == false then
        local px0 = LFX_BinaryString(4)
        local py0 = LFX_BinaryString(4)
        local px1 = LFX_BinaryString(4)
        local py1 = LFX_BinaryString(4)
        stbtt_GetGlyphBox(self.font, glyph.glyph_index, px0, py0, px1, py1)
        local x0 = string.unpack("i", px0)
        local y0 = string.unpack("i", py0)
        local x1 = string.unpack("i", px1)
        local y1 = string.unpack("i", py1)
        glyph.box = { x0, y0, x1, y1 },

        stbtt_GetGlyphBitmapBoxSubpixel(self.font, glyph.glyph_index, self.font_scale, self.font_scale, 0, 0, px0, py0, px1, py1)
        x0 = string.unpack("i", px0)
        y0 = string.unpack("i", py0)
        x1 = string.unpack("i", px1)
        y1 = string.unpack("i", py1)
        glyph.bitmap_box = { x0, y0, x1, y1 }

        local w = x1 - x0
        local h = y1 - y0
        local bitmap = LFX_Malloc(w * h)
        stbtt_MakeGlyphBitmapSubpixel(self.font, bitmap, w, h, w, self.font_scale, self.font_scale, 0, 0, glyph.glyph_index)
        glyph.texture_rect = self:FillTextureRect(bitmap, w, h)
        LFX_Free(bitmap)
    end

    return glyph
end

function Font:GetGlyphKernAdvance(code1, code2)
    local glyph1 = self:GetGlyph(code1)
    local glyph2 = self:GetGlyph(code2)
    if glyph1 and glyph1.glyph_index > 0 and glyph2 and glyph2.glyph_index > 0 then
        return stbtt_GetGlyphKernAdvance(self.font, glyph1.glyph_index, glyph2.glyph_index)
    end
    return 0
end

function Font:GetTexture()
    return self.texture
end

-- private
function Font:FillTextureRect(bitmap, w, h)
    local rect = { 0, 0, 0, 0 }
        
    if self.texture == nil then
        self.texture = gl.CreateTexture(GL_TEXTURE_2D, self.texture_format, self.texture_size[1], self.texture_size[2], GL_NEAREST, GL_CLAMP_TO_EDGE)
    end

    if w > self.texture.width then
        LOGE("Font.FillTextureRect over texture width")
        return rect
    end

    local x = self.texture_pos[1]
    local y = self.texture_pos[2]

    if x + w > self.texture.width then
        x = 0
        y = self.texture_pos_y_max + 1
    end

    if y + h > self.texture.height then
        LOGE("Font.FillTextureRect over texture height")
        return rect
    end

    gl.UpdateTexture(self.texture, x, y, w, h, bitmap)

    self.texture_pos_y_max = math.max(self.texture_pos_y_max, y + h)
    self.texture_pos = { x + w + 1, y }

    rect = { x / self.texture.width, y / self.texture.height, w / self.texture.width, h / self.texture.height }

    return rect
end

return Font
