local gl = require("gl")

local Canvas = { }

-- public
function Canvas.New()
    local o = { }
    setmetatable(o, Canvas)
    Canvas.__index = Canvas
    return o
end

function Canvas:Init(context, width, height)
    self.context = context
    self.width = width
    self.height = height
end

function Canvas:Done()
    
end

function Canvas:DrawText(str, font, x, y)
    local codes = { }
    for pos, code in utf8.codes(str) do
        codes[#codes + 1] = code
    end
    for i = 1, #codes do
        local code = codes[i]
        local glyph = font:GetGlyph(code)
    end
    LFX_Context_RenderQuad(self.context, font:GetTexture(), LFX_MAT4_FLIP_Y)
end

return Canvas
