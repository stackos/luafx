local gl = require("gl")
local Font = require("Font")
local Canvas = require("Canvas")

local _font = nil
local _canvas = nil
local _label = nil

local DemoInit = function(context, effect)
    LOGD("DemoInit demo_text")

    local font_path = LFX_Effect_GetEffectDir(effect) .. "/font/STXINWEI.TTF"
    _font = Font.New()
    _font:Init(context, font_path, 20)
end

local DemoDone = function(context, effect)
    LOGD("DemoDone")

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
    _canvas:DrawText(_label, 0, 20, { 1, 1, 1, 1 })
end

return {
    Init = DemoInit,
    Done = DemoDone,
    Render = DemoRender
}
