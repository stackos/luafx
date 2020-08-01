local gl = require("gl")

local _vbo = nil
local _ibo = nil
local _program = nil

local DemoInit = function(context, effect)
    LOGD("DemoInit demo_triangle")

    -- vbo
    local vertices = LFX_Float32ArrayCreateFromTable({
        0, 0.5,         1, 0, 0, 1,
        -0.5, -0.5,     0, 1, 0, 1,
        0.5, -0.5,      0, 0, 1, 1,
    })
    local indices = LFX_Uint16ArrayCreateFromTable({
        0, 1, 2
    })

    _vbo = LFX_BinaryString(4)
    glGenBuffers(1, _vbo)
    local vbo = string.unpack("i", _vbo)

    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBufferData(GL_ARRAY_BUFFER, 4 * 6 * 3, vertices, GL_STATIC_DRAW)
    glBindBuffer(GL_ARRAY_BUFFER, 0)

    -- ibo
    _ibo = LFX_BinaryString(4)
    glGenBuffers(1, _ibo)
    local ibo = string.unpack("i", _ibo)

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3, indices, GL_STATIC_DRAW)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)

    LFX_Free(vertices)
    LFX_Free(indices)

    -- program
    local vs = [[
        uniform mat4 uMatrix;
        attribute vec2 aPosition;
        attribute vec4 aColor;
        varying vec4 vColor;
        void main()
        {
            gl_Position = uMatrix * vec4(aPosition, 0.0, 1.0);
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
end

local DemoRender = function(context, effect, input_texture, output_texture)
    local vbo = string.unpack("i", _vbo)
    local ibo = string.unpack("i", _ibo)

    -- draw
    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)

    gl.UseProgram(_program)
    gl.UniformMatrix(_program, "uMatrix", LFX_MAT4_FLIP_Y)
    gl.VertexAttrib(_program, "aPosition", 2, 4 * 6, 0)
    gl.VertexAttrib(_program, "aColor", 4, 4 * 6, 4 * 2)

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0)

    -- restore
    gl.DisableVertexAttribs(_program)
end

return {
    Init = DemoInit,
    Done = DemoDone,
    Render = DemoRender
}
