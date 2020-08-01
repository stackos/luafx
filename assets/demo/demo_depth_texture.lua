local gl = require("gl")

local _vbo = nil
local _ibo = nil
local _program = nil
local _depth_texture = nil
local _rot = 0
local _depth_format = 0

local DemoInit = function(context, effect)
    LOGD("DemoInit demo_depth_texture")

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
        uniform mat4 uMatrix;
        attribute vec3 aPosition;
        attribute vec4 aColor;
        varying vec4 vColor;
        void main()
        {
            gl_Position = uMatrix * vec4(aPosition, 1.0);
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
    if string.unpack("i", major) >= 3 then
        _depth_format = GL_DEPTH_COMPONENT24
        LOGD("depth format: GL_DEPTH_COMPONENT24")
    else
        if LFX_Context_CheckGLExtension(context, "GL_OES_depth_texture") == LFX_SUCCESS then
            _depth_format = GL_DEPTH_COMPONENT
            LOGD("depth format: GL_DEPTH_COMPONENT")
        else
            _depth_format = 0
            LOGE("not support depth texture")
        end
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

    if _depth_texture then
        glDeleteTextures(1, _depth_texture.tex)
    end
end

local function BindDepthBuffer(output_texture)
    if _depth_texture == nil then
        _depth_texture = {
            id = 0,
            target = GL_TEXTURE_2D,
            format = _depth_format,
            width = 0,
            height = 0,
            filter_mode = GL_NEAREST,
            wrap_mode = GL_CLAMP_TO_EDGE,
            tex = LFX_BinaryString(4)
        }
        glGenTextures(1, _depth_texture.tex)
        _depth_texture.id = string.unpack("i", _depth_texture.tex)
    end
    if _depth_texture.width ~= output_texture.width or _depth_texture.height ~= output_texture.height then
        _depth_texture.width = output_texture.width
        _depth_texture.height = output_texture.height
        glBindTexture(_depth_texture.target, _depth_texture.id)
        glTexImage2D(_depth_texture.target, 0, _depth_texture.format, _depth_texture.width, _depth_texture.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nil)
        glTexParameteri(_depth_texture.target, GL_TEXTURE_MIN_FILTER, _depth_texture.filter_mode)
        glTexParameteri(_depth_texture.target, GL_TEXTURE_MAG_FILTER, _depth_texture.filter_mode)
        glTexParameteri(_depth_texture.target, GL_TEXTURE_WRAP_S, _depth_texture.wrap_mode)
        glTexParameteri(_depth_texture.target, GL_TEXTURE_WRAP_T, _depth_texture.wrap_mode)
        glBindTexture(_depth_texture.target, 0)
    end
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depth_texture.target, _depth_texture.id, 0)

    if glCheckFramebufferStatus(GL_FRAMEBUFFER) ~= GL_FRAMEBUFFER_COMPLETE then
        LOGE("framebuffer status not complete")
    end
end

local DemoRender = function(context, effect, input_texture, output_texture)
    local vbo = string.unpack("i", _vbo)
    local ibo = string.unpack("i", _ibo)

    -- mvp
    local view = mat4()
    glm_lookat_lh(vec3(0, 2, -4), vec3(0, 0, 0), vec3(0, 1, 0), view)
    local proj = mat4()
    glm_perspective_lh(glm_rad(45), output_texture.width / output_texture.height, 2, 100, proj)
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
    gl.UniformMatrix(_program, "uMatrix", mvp)
    gl.VertexAttrib(_program, "aPosition", 3, 4 * 7, 0)
    gl.VertexAttrib(_program, "aColor", 4, 4 * 7, 4 * 3)

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0)

    -- restore
    gl.DisableVertexAttribs(_program)

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depth_texture.target, 0, 0)

    glDisable(GL_CULL_FACE)
    glDisable(GL_DEPTH_TEST)

    -- draw quad with depth texture
    LFX_Context_RenderQuad(context, _depth_texture, LFX_MAT4_FLIP_Y)
end

return {
    Init = DemoInit,
    Done = DemoDone,
    Render = DemoRender
}
