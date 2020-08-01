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
