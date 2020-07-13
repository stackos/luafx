local gl = require("gl")

local demo = require("demo/demo_triangle")
--local demo = require("demo/demo_cube")
--local demo = require("demo/demo_depth_texture")
--local demo = require("demo/demo_instance")

local _effect_data = {
    timestamp = 0,
    shared_quad_renderer = require("QuadRenderer"):New(),

    Init = function(self, context, effect)
        self.shared_quad_renderer:Init(context, effect)

        demo.Init(context, effect)
    end,

    Done = function(self, context, effect)
        self.shared_quad_renderer:Done(context, effect)

        demo.Done(context, effect)
    end,

    Render = function(self, context, effect, input_texture, output_texture)
        gl.AttachColor(output_texture)
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
    _effect_data:Init(context, effect)
    return LFX_SUCCESS
end

function Effect_Done(context, effect)
    LOGD("Effect_Done")
    _effect_data:Done(context, effect)
    return LFX_SUCCESS
end

function Effect_Render(context, effect, input_texture, output_texture)
    _effect_data:Render(context, effect, input_texture, output_texture)
    return LFX_SUCCESS
end

function Effect_SendMessage(context, effect, message_id, message)
    if message_id == LFX_MESSAGE_ID_SET_EFFECT_TIMESTAMP then
        _effect_data:SetTimestamp(context, effect, message)
    end
end
