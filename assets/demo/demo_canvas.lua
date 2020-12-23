local gl = require("gl")
local Font = require("Font")
local Canvas = require("Canvas")

local _font = nil
local _canvas = nil
local _label = nil
local _texture = nil
local _texture2 = nil

local DemoInit = function(context, effect)
    LOGD("DemoInit demo_canvas")

    local font_path = LFX_Effect_GetEffectDir(effect) .. "/font/STXINWEI.TTF"
    _font = Font.New()
    _font:Init(context, font_path, 20, 2)

    _texture = gl.LoadTexture(context, LFX_Effect_GetEffectDir(effect) .. "/input/1080x1920.jpg", GL_LINEAR, GL_CLAMP_TO_EDGE)
    _texture2 = gl.LoadTexture(context, LFX_Effect_GetEffectDir(effect) .. "/input/1280x720.jpg", GL_LINEAR, GL_CLAMP_TO_EDGE)
end

local DemoDone = function(context, effect)
    LOGD("DemoDone")

    if _texture then
        gl.DestroyTexture(context, _texture)
    end
    if _texture2 then
        gl.DestroyTexture(context, _texture2)
    end
    _font:Done()
    if _canvas then
        if _label then
            _canvas:DestroyText(_label)
        end
        _canvas:Done()
    end
end

local DemoRender = function(context, effect, input_texture, output_texture)
    if _canvas == nil or _canvas.width ~= output_texture.width or _canvas.height ~= output_texture.height then
        if _canvas then
            _canvas:Done()
            _canvas = nil
        end
        _canvas = Canvas.New()
        _canvas:Init(context, output_texture.width, output_texture.height)
        _label = _canvas:CreateText("Hello World!\n你好，世界！", _font)
    end

    _canvas:DrawBegin()
    _canvas:DrawTexture(_texture2, output_texture.width / 2, output_texture.height / 2, output_texture.width, output_texture.height)
    for i = 1, 10 do
        for j = 1, 5 do
            _canvas:DrawTexture(_texture,
                100 + 120 * (i - 1), 100 + 120 * (j - 1),
                100, 100,
                (i - 1) * 100, 300 + (j - 1) * 100,
                100, 100,
                0,
                { 1, (j - 1) / 5, (i - 1) / 10, 1 })
        end
    end
    _canvas:DrawText(_label, 0, 20, { 1, 1, 1, 1 }, { 0, 0, 0, 1 })
    --_canvas:DrawTexture(_font:GetTexture(), 512, 512, 1024, 1024, 0, 0, 1024, 1024)
    _canvas:DrawEnd()
end

return {
    Init = DemoInit,
    Done = DemoDone,
    Render = DemoRender
}
