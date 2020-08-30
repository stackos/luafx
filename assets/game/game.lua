local gl = require("gl")
local Font = require("Font")
local Canvas = require("Canvas")

local Atlas = {
    Bottom = { 642, 214, 516, 185 },
    BottomExt = { 642, 307, 516, 92 },
    Circle = { 36, 36, 128, 128 },
}

local Game = { }

function Game.New()
    local o = { }
    setmetatable(o, Game)
    Game.__index = Game
    return o
end

function Game:Init(context, effect)
    self.context = context
    self.effect = effect
    self.texture = gl.LoadTexture(self.context, LFX_Effect_GetEffectDir(self.effect) .. "/game/canvas.png", GL_LINEAR, GL_CLAMP_TO_EDGE)
    self.canvas = nil
end

function Game:Done()
    gl.DestroyTexture(self.context, self.texture)
    if self.canvas then
        self.canvas:Done()
    end
end

function Game:Render(input_texture, output_texture)
    if self.canvas == nil or self.canvas.width ~= output_texture.width or self.canvas.height ~= output_texture.height then
        if self.canvas then
            self.canvas:Done()
            self.canvas = nil
        end
        self.canvas = Canvas.New()
        self.canvas:Init(self.context, output_texture.width, output_texture.height)
    end

    self.canvas:DrawBegin()
    self:DrawMainUI()
    self.canvas:DrawEnd()
end

function Game:DrawMainUI()
    self:DrawBottom()
end

function Game:DrawBottom()
    local w = self.canvas.width
    local scale = w / Atlas.Bottom[3]
    local h = Atlas.Bottom[4] * scale
    local x = self.canvas.width / 2
    local y = self.canvas.width * 16 / 9 - h / 2 + w * 0.15
    local bottom = y + h / 2

    self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.Bottom[1], Atlas.Bottom[2], Atlas.Bottom[3], Atlas.Bottom[4])
    
    if self.canvas.height >= bottom then
        h = self.canvas.height - bottom
        y = self.canvas.height - h / 2

        self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.BottomExt[1], Atlas.BottomExt[2], Atlas.BottomExt[3], Atlas.BottomExt[4])
    end

    local circle_y = bottom - 1150 * scale
    local circle_r = 1000 * scale
    local circle_line_w = 4

    -- circle test
    do
        local r = 15 * scale
        w = r
        h = r
        if self.deg == nil then
            self.deg = -15
        end
        self.deg = self.deg + 0.1
        if self.deg > 15 then
            self.deg = -15
        end
        x = self.canvas.width / 2 + circle_r * math.sin(self.deg * math.pi / 180)
        y = circle_y + circle_r * math.cos(self.deg * math.pi / 180) - r / 2 - circle_line_w / 2
        self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.Circle[1], Atlas.Circle[2], Atlas.Circle[3], Atlas.Circle[4])
    end
end

local game = nil

return {
    Init = function(context, effect)
        game = Game.New()
        game:Init(context, effect)
    end,

    Done = function(context, effect)
        game:Done()
        game = nil
    end,

    Render = function(context, effect, input_texture, output_texture)
        game:Render(input_texture, output_texture)
    end,
}
