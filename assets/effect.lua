require("table_ext")
local gl = require("gl")
local Font = require("Font")
local Canvas = require("Canvas")

local BuildPlatformNames = {
    "Windows",
    "Android",
    "Mac",
    "iOS",
    "WebAssembly"
}

local _effect = {
    timestamp = 0,
    demo = require("demo/demo_triangle"),
    --demo = require("demo/demo_cube"),
    --demo = require("demo/demo_depth_texture"),
    --demo = require("demo/demo_instance"),
    font = nil,

    Init = function(self, context, effect)
        local build_platform = LFX_BinaryString(4)
        LFX_Context_GetBuildPlatform(context, build_platform)
        local platform = string.unpack("i", build_platform)

        LOGD(string.format("BuildPlatform: %s", BuildPlatformNames[platform + 1]))

        self.demo.Init(context, effect)

        -- test canvas
        local font_path = "c:/windows/fonts/msyh.ttc"
        if platform == LFX_BUILD_PLATFORM_MAC then
            font_path = "/System/Library/Fonts/PingFang.ttc"
        end
        self.font = Font.New()
        self.font:Init(context, font_path, 20)
        self.canvas = nil
    end,

    Done = function(self, context, effect)
        self.demo.Done(context, effect)

        self.font:Done()
        if self.canvas then
            self.canvas:Done()
        end
    end,

    Render = function(self, context, effect, input_texture, output_texture)
        gl.AttachColorTexture(output_texture)
        LFX_Context_RenderQuad(context, input_texture, LFX_MAT4_FLIP_Y)

        self.demo.Render(context, effect, input_texture, output_texture)

        -- test canvas
        if self.canvas == nil or self.canvas.width ~= output_texture.width or self.canvas.height ~= output_texture.height then
            if self.canvas then
                self.canvas:Done()
                self.canvas = nil
            end
            self.canvas = Canvas.New()
            self.canvas:Init(context, output_texture.width, output_texture.height)
        end

        self.canvas:DrawText("Hello World! 你好世界！", self.font, 0, 20)
    end,

    SetTimestamp = function(self, context, effect, message)
        local msg = LFX_JsonParse(message)
        self.timestamp = math.floor(msg.timestamp)
    end,
}

function Effect_Load(context, effect)
    LOGD("Effect_Load")
    _effect:Init(context, effect)
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
