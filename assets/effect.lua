require("table_ext")
local gl = require("gl")

local BuildPlatformNames = {
    "Windows",
    "Android",
    "Mac",
    "iOS",
    "WebAssembly"
}

local _effect = {
    timestamp = 0,
    --demo = require("demo/demo_triangle"),
    --demo = require("demo/demo_cube"),
    --demo = require("demo/demo_depth_texture"),
    --demo = require("demo/demo_instance"),
    demo = require("demo/demo_canvas"),
    --demo = require("game/game"),

    Init = function(self, context, effect)
        local build_platform = LFX_BinaryString(4)
        LFX_Context_GetBuildPlatform(context, build_platform)
        local platform = string.unpack("i", build_platform)

        LOGD(string.format("BuildPlatform: %s", BuildPlatformNames[platform + 1]))

        self.demo.Init(context, effect)
    end,

    Done = function(self, context, effect)
        self.demo.Done(context, effect)
    end,

    Render = function(self, context, effect, input_texture, output_texture)
        gl.AttachColorTexture(output_texture)
        if input_texture then
            LFX_Context_RenderQuad(context, input_texture, LFX_MAT4_FLIP_Y)
        else
            glClearColor(0, 0, 0, 0)
            glClear(GL_COLOR_BUFFER_BIT)
        end

        self.demo.Render(context, effect, input_texture, output_texture)
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
