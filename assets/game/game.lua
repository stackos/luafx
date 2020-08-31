local gl = require("gl")
local Font = require("Font")
local Canvas = require("Canvas")

local Atlas = {
    Top = { 196, 200, 408, 204 },
    Bottom = { 642, 214, 516, 185 },
    BottomExt = { 642, 307, 516, 92 },
    Circle = { 36, 36, 128, 128 },
    RoundLineLeft = { 99 - 16, 300 - 16, 16, 32 },
    RoundLineCenter = { 99, 300 - 16, 2, 32 },
    RoundLineRight = { 101, 300 - 16, 16, 32 },
    Fan = { 300 - 64, 500 - 64, 128, 128 },
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
    self.fan_deg = 0
    self.bottom_ground = nil
    self.bg_texture = nil
    self.bg_fbo = nil
end

function Game:Done()
    if self.bg_fbo then
        glDeleteFramebuffers(1, self.bg_fbo)
    end
    if self.bg_texture then
        gl.DestroyTexture(self.context, self.bg_texture)
    end
    if self.texture then
        gl.DestroyTexture(self.context, self.texture)
    end
    if self.canvas then
        self.canvas:Done()
    end
end

function Game:Render(input_texture, output_texture)
    self:Update()

    if self.canvas == nil or self.canvas.width ~= output_texture.width or self.canvas.height ~= output_texture.height then
        if self.canvas then
            self.canvas:Done()
            self.canvas = nil
        end
        self.canvas = Canvas.New()
        self.canvas:Init(self.context, output_texture.width, output_texture.height)
    end

    self:PreDrawMainUI()

    self.canvas:DrawBegin()
    self:DrawMainUI()
    self:DrawFan()
    self:DrawBall()
    self.canvas:DrawEnd()
end

function Game:Update()
    self.fan_deg = self.fan_deg + 8
end

function Game:PreDrawMainUI()
    if self.bg_texture == nil or self.bg_texture.width ~= self.canvas.width or self.bg_texture.height ~= self.canvas.height then
        if self.bg_texture then
            gl.DestroyTexture(self.context, self.bg_texture)
        end
        self.bg_texture = gl.CreateTexture(GL_TEXTURE_2D, GL_RGBA, self.canvas.width, self.canvas.height, GL_NEAREST, GL_CLAMP_TO_EDGE)

        local old_fbo = LFX_BinaryString(4)
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, old_fbo)

        if self.bg_fbo == nil then
            self.bg_fbo = LFX_BinaryString(4)
            glGenFramebuffers(1, self.bg_fbo)
        end
        glBindFramebuffer(GL_FRAMEBUFFER, string.unpack("i", self.bg_fbo))
        gl.AttachColorTexture(self.bg_texture)
        glClearColor(0, 0, 0, 0)
        glClear(GL_COLOR_BUFFER_BIT)

        self.canvas:DrawBegin()
        self:DrawTop()
        self:DrawBottom()
        self.canvas:DrawEnd()

        glBindFramebuffer(GL_FRAMEBUFFER, string.unpack("i", old_fbo))
    end
end

function Game:DrawMainUI()
    LFX_Context_RenderQuad(self.context, self.bg_texture, LFX_MAT4_FLIP_Y)
end

function Game:DrawTop()
    local gray = 86 / 255.0
    local w = self.canvas.width * 0.4
    local scale = w / Atlas.Top[3]
    local h = Atlas.Top[4] * scale
    local x = self.canvas.width * 0.06 + w / 2
    local y = self.canvas.width * 0.05 + h / 2
    -- left top
    self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.Top[1], Atlas.Top[2], Atlas.Top[3], Atlas.Top[4], 0, { gray, gray, gray, 1 })

    x = self.canvas.width * (1.0 - 0.06) - w / 2
    w = -w
    -- right top
    self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.Top[1], Atlas.Top[2], Atlas.Top[3], Atlas.Top[4], 0, { gray, gray, gray, 1 })

    local top = y + h / 2
    w = self.canvas.width * 1.32
    h = self.canvas.width * 0.0076
    x = self.canvas.width * 0.06 + h / 2
    y = top + w / 2 - 4
    -- left wall
    self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.RoundLineCenter[1], Atlas.RoundLineCenter[2], Atlas.RoundLineCenter[3], Atlas.RoundLineCenter[4], 90, { gray, gray, gray, 1 })

    local bottom = y + w / 2
    scale = h / Atlas.RoundLineRight[4]
    w = Atlas.RoundLineRight[3] * scale
    y = bottom + w / 2
    -- left wall end
    self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.RoundLineRight[1], Atlas.RoundLineRight[2], Atlas.RoundLineRight[3], Atlas.RoundLineRight[4], -90, { gray, gray, gray, 1 })

    w = self.canvas.width * 1.32
    x = self.canvas.width * (1.0 - 0.06) - h / 2
    y = top + w / 2 - 4
    -- right wall
    self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.RoundLineCenter[1], Atlas.RoundLineCenter[2], Atlas.RoundLineCenter[3], Atlas.RoundLineCenter[4], 90, { gray, gray, gray, 1 })

    w = Atlas.RoundLineRight[3] * scale
    y = bottom + w / 2
    -- right wall end
    self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.RoundLineRight[1], Atlas.RoundLineRight[2], Atlas.RoundLineRight[3], Atlas.RoundLineRight[4], -90, { gray, gray, gray, 1 })
end

function Game:DrawBottom()
    local w = self.canvas.width
    local scale = w / Atlas.Bottom[3]
    local h = Atlas.Bottom[4] * scale
    local x = self.canvas.width / 2
    local y = self.canvas.width * 16 / 9 - h / 2 + w * 0.2
    local bottom = y + h / 2

    self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.Bottom[1], Atlas.Bottom[2], Atlas.Bottom[3], Atlas.Bottom[4])
    
    if self.canvas.height >= bottom then
        h = self.canvas.height - bottom
        y = self.canvas.height - h / 2

        self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.BottomExt[1], Atlas.BottomExt[2], Atlas.BottomExt[3], Atlas.BottomExt[4])
    end

    self.bottom_ground = {
        circle_y = bottom - 1150 * scale,
        circle_r = 1000 * scale,
        circle_line_w = 4,
    }
end

function Game:DrawFan()
    local gray = 39 / 255.0
    local w = self.canvas.width * 0.05
    local scale = w / Atlas.Fan[3]
    local h = Atlas.Fan[4] * scale
    local x = self.canvas.width * 0.032
    local y = self.canvas.width * 1.59
    -- left fan
    self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.Fan[1], Atlas.Fan[2], Atlas.Fan[3], Atlas.Fan[4], -self.fan_deg, { gray, gray, gray, 1 })

    x = self.canvas.width * (1.0 - 0.032)
    -- right fan
    self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.Fan[1], Atlas.Fan[2], Atlas.Fan[3], Atlas.Fan[4], self.fan_deg, { gray, gray, gray, 1 })
end

function Game:DrawBall()
    -- circle test
    local r = self.canvas.width * 0.03
    local w = r
    local h = r
    if self.deg == nil then
        self.deg = -15
    end
    self.deg = self.deg + 0.1
    if self.deg > 15 then
        self.deg = -15
    end
    local x = self.canvas.width / 2 + self.bottom_ground.circle_r * math.sin(self.deg * math.pi / 180)
    local y = self.bottom_ground.circle_y + self.bottom_ground.circle_r * math.cos(self.deg * math.pi / 180) - r / 2 - self.bottom_ground.circle_line_w / 2
    self.canvas:DrawTexture(self.texture, x, y, w, h, Atlas.Circle[1], Atlas.Circle[2], Atlas.Circle[3], Atlas.Circle[4])
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
