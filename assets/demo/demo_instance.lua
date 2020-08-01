local gl = require("gl")

local _vbo = nil
local _ibo = nil
local _program = nil
local _rbo = nil
local _rbo_size = {
    width = 0,
    height = 0
}
local _rot = 0
local _depth_format = GL_DEPTH_COMPONENT16
local _support_instance = false
local _support_instance_ext = false
local _vbo_instance = nil

local DemoInit = function(context, effect)
    LOGD("DemoInit demo_instance")

    -- vbo
    local vertices = LFX_Float32ArrayCreateFromTable({
        -0.5, 0.5, -0.5,    1, 0, 0, 1,
        -0.5, -0.5, -0.5,   0, 1, 0, 1,
        0.5, -0.5, -0.5,    0, 0, 1, 1,
        0.5, 0.5, -0.5,     0, 1, 1, 1,
        -0.5, 0.5, 0.5,     1, 0, 1, 1,
        -0.5, -0.5, 0.5,    1, 1, 0, 1,
        0.5, -0.5, 0.5,     0, 0, 0, 1,
        0.5, 0.5, 0.5,      1, 1, 1, 1,
    })
    local indices = LFX_Uint16ArrayCreateFromTable({
        0, 1, 2, 0, 2, 3,
        3, 2, 6, 3, 6, 7,
        7, 6, 5, 7, 5, 4,
        4, 5, 1, 4, 1, 0,
        4, 0, 3, 4, 3, 7,
        1, 5, 6, 1, 6, 2,
    })

    _vbo = LFX_BinaryString(4)
    glGenBuffers(1, _vbo)
    local vbo = string.unpack("i", _vbo)

    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBufferData(GL_ARRAY_BUFFER, 4 * 7 * 8, vertices, GL_STATIC_DRAW)
    glBindBuffer(GL_ARRAY_BUFFER, 0)

    -- ibo
    _ibo = LFX_BinaryString(4)
    glGenBuffers(1, _ibo)
    local ibo = string.unpack("i", _ibo)

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 6 * 6, indices, GL_STATIC_DRAW)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)

    LFX_Free(vertices)
    LFX_Free(indices)

    -- program
    local vs = [[
        uniform mat4 uVP;
        uniform mat4 uModel;
        attribute vec3 aPosition;
        attribute vec4 aColor;
        attribute vec3 aPositionInstance;
        varying vec4 vColor;
        void main()
        {
            mat4 pos_instance = mat4(
                vec4(1.0, 0.0, 0.0, 0.0),
                vec4(0.0, 1.0, 0.0, 0.0),
                vec4(0.0, 0.0, 1.0, 0.0),
                vec4(aPositionInstance, 1.0)
                );
            gl_Position = uVP * pos_instance * uModel * vec4(aPosition, 1.0);
            vColor = aColor;
        }
        ]]
    local fs = [[
        precision highp float;
        varying vec4 vColor;
        void main()
        {
            gl_FragColor = vColor;
        }
        ]]

    _program = gl.CreateProgram(context, vs, fs)

    -- depth format
    local major = LFX_BinaryString(4)
    local minor = LFX_BinaryString(4)
    local is_es = LFX_BinaryString(4)
    LFX_Context_GetGLVersion(context, major, minor, is_es)
    local gl_version = {
        major = string.unpack("i", major),
        minor = string.unpack("i", minor),
        is_es = string.unpack("i", is_es),
    }

    if gl_version.major >= 3 then
        _depth_format = GL_DEPTH_COMPONENT24
        LOGD("depth format: GL_DEPTH_COMPONENT24")
    else
        if LFX_Context_CheckGLExtension(context, "GL_OES_depth24") == LFX_SUCCESS then
            _depth_format = GL_DEPTH_COMPONENT24_OES
            LOGD("depth format: GL_DEPTH_COMPONENT24_OES")
        else
            _depth_format = GL_DEPTH_COMPONENT16
            LOGD("depth format: GL_DEPTH_COMPONENT16")
        end
    end

    -- instance
    if gl_version.is_es then
        if gl_version.major >= 3 then
            _support_instance = true
            LOGD("support instance")
        elseif gl_version.major == 2 then
            if LFX_Context_CheckGLExtension(context, "GL_EXT_instanced_arrays") == LFX_SUCCESS and
               LFX_Context_CheckGLExtension(context, "GL_EXT_draw_instanced") == LFX_SUCCESS then
                _support_instance_ext = true
                LOGD("support instance ext")
            end
        end
    else
        if (gl_version.major == 3 and gl_version.minor >= 3) or (gl_version.major > 3) then
            _support_instance = true
            LOGD("support instance")
        end
    end

    if _support_instance or _support_instance_ext then
        local positions = { }
        for i = 1, 40 do
            for j = 1, 40 do
                positions[((i - 1) * 40 + j - 1) * 3 + 1] = j * 1.5
                positions[((i - 1) * 40 + j - 1) * 3 + 2] = i * 1.5
                positions[((i - 1) * 40 + j - 1) * 3 + 3] = 0
            end
        end
        local positions_array = LFX_Float32ArrayCreateFromTable(positions)

        _vbo_instance = LFX_BinaryString(4)
        glGenBuffers(1, _vbo_instance)
        local vbo_instance = string.unpack("i", _vbo_instance)

        glBindBuffer(GL_ARRAY_BUFFER, vbo_instance)
        glBufferData(GL_ARRAY_BUFFER, 4 * 40 * 40 * 3, positions_array, GL_STATIC_DRAW)
        glBindBuffer(GL_ARRAY_BUFFER, 0)

        LFX_Free(positions_array)
    else
        LOGD("not support instance")
    end
end

local DemoDone = function(context, effect)
    LOGD("DemoDone")

    if _vbo then
        glDeleteBuffers(1, _vbo)
    end

    if _ibo then
        glDeleteBuffers(1, _ibo)
    end

    if _program then
        gl.DeleteProgram(_program)
    end

    if _rbo then
        glDeleteRenderbuffers(1, _rbo)
    end

    if _vbo_instance then
        glDeleteBuffers(1, _vbo_instance)
    end
end

local function BindDepthBuffer(output_texture)
    if _rbo == nil then
        _rbo = LFX_BinaryString(4)
        glGenRenderbuffers(1, _rbo)
    end
    local rbo = string.unpack("i", _rbo)
    if _rbo_size.width ~= output_texture.width or _rbo_size.height ~= output_texture.height then
        _rbo_size.width = output_texture.width
        _rbo_size.height = output_texture.height
        glBindRenderbuffer(GL_RENDERBUFFER, rbo)
        glRenderbufferStorage(GL_RENDERBUFFER, _depth_format, _rbo_size.width, _rbo_size.height)
        glBindRenderbuffer(GL_RENDERBUFFER, 0)
    end
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo)

    if glCheckFramebufferStatus(GL_FRAMEBUFFER) ~= GL_FRAMEBUFFER_COMPLETE then
        LOGE("framebuffer status not complete")
    end
end

local DemoRender = function(context, effect, input_texture, output_texture)
    local vbo = string.unpack("i", _vbo)
    local ibo = string.unpack("i", _ibo)

    -- mvp
    local view = mat4()
    glm_lookat_lh(vec3(30, 40, -80), vec3(30, 30, 0), vec3(0, 1, 0), view)
    local proj = mat4()
    glm_perspective_lh(glm_rad(45), output_texture.width / output_texture.height, 0.3, 200, proj)
    local vp = mat4()
    glm_mat4_mul(proj, view, vp)
    glm_mat4_mul(LFX_MAT4_FLIP_Y, vp, vp)

    _rot = _rot + 1
    local rotate = mat4()
    glm_euler_zxy(vec3(0, glm_rad(_rot), 0), rotate)
    local translate = mat4()
    glm_translate_make(translate, vec3(0, 0, 0))
    local model = mat4()
    glm_mat4_mul(translate, rotate, model)

    local mvp = mat4()
    glm_mat4_mul(vp, model, mvp)

    -- state
    glEnable(GL_CULL_FACE)
    glEnable(GL_DEPTH_TEST)

    -- depth rbo
    BindDepthBuffer(output_texture)

    glClear(GL_DEPTH_BUFFER_BIT)

    -- draw
    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)

    gl.UseProgram(_program)
    gl.UniformMatrix(_program, "uVP", vp)
    gl.UniformMatrix(_program, "uModel", model)
    gl.VertexAttrib(_program, "aPosition", 3, 4 * 7, 0)
    gl.VertexAttrib(_program, "aColor", 4, 4 * 7, 4 * 3)

    if _support_instance or _support_instance_ext then
        local vbo_instance = string.unpack("i", _vbo_instance)
        glBindBuffer(GL_ARRAY_BUFFER, vbo_instance)

        gl.VertexAttrib(_program, "aPositionInstance", 3, 0, 0)

        local VertexAttribDivisor = nil
        local DrawElementsInstanced = nil
        if _support_instance then
            VertexAttribDivisor = glVertexAttribDivisor
            DrawElementsInstanced = glDrawElementsInstanced
        elseif _support_instance_ext then
            VertexAttribDivisor = glVertexAttribDivisorEXT
            DrawElementsInstanced = glDrawElementsInstancedEXT
        end

        VertexAttribDivisor(_program.attributes["aPositionInstance"].location, 1)
        DrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0, 40 * 40)
        VertexAttribDivisor(_program.attributes["aPositionInstance"].location, 0)
    end

    -- restore
    gl.DisableVertexAttribs(_program)

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0)

    glDisable(GL_CULL_FACE)
    glDisable(GL_DEPTH_TEST)
end

return {
    Init = DemoInit,
    Done = DemoDone,
    Render = DemoRender
}
