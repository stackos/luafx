local gl = require("gl")

local QuadRenderer = { }

function QuadRenderer.New()
    local o = { }
    setmetatable(o, QuadRenderer)
    QuadRenderer.__index = QuadRenderer
    return o
end

function QuadRenderer:Init(context)
    self.context = context
    self.vbo = nil
    self.ibo = nil
    self.program = nil

    -- vbo
    local vertices = LFX_Float32ArrayCreateFromTable({
        -1, 1,      0, 0,
        -1, -1,     0, 1,
        1, -1,      1, 1,
        1, 1,       1, 0,
    })
    local indices = LFX_Uint16ArrayCreateFromTable({
        0, 1, 2, 0, 2, 3
    })

    self.vbo = LFX_BinaryString(4)
    glGenBuffers(1, self.vbo)
    local vbo = string.unpack("i", self.vbo)

    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * 4, vertices, GL_STATIC_DRAW)
    glBindBuffer(GL_ARRAY_BUFFER, 0)

    -- ibo
    self.ibo = LFX_BinaryString(4)
    glGenBuffers(1, self.ibo)
    local ibo = string.unpack("i", self.ibo)

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 6, indices, GL_STATIC_DRAW)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)

    LFX_Free(vertices)
    LFX_Free(indices)

    -- program
    local vs = [[
        uniform mat4 uMatrix;
        uniform vec4 uTextureCoordScaleOffset;
        attribute vec2 aPosition;
        attribute vec2 aTextureCoord;
        varying vec2 vTextureCoord;
        void main()
        {
            gl_Position = uMatrix * vec4(aPosition, 0.0, 1.0);
            vTextureCoord = aTextureCoord * uTextureCoordScaleOffset.xy + uTextureCoordScaleOffset.zw;
        }
        ]]
    local fs = [[
        precision highp float;
        uniform sampler2D uTexture0;
        uniform vec4 uColor;
        varying vec2 vTextureCoord;
        void main()
        {
            gl_FragColor = texture2D(uTexture0, vTextureCoord) * uColor;
        }
        ]]

    self.program = gl.CreateProgram(self.context, vs, fs)
end

function QuadRenderer:Done()
    if self.vbo then
        glDeleteBuffers(1, self.vbo)
    end

    if self.ibo then
        glDeleteBuffers(1, self.ibo)
    end

    if self.program then
        gl.DestroyProgram(self.program)
    end
end

function QuadRenderer:Render(texture, mvp, program, uv_scale_offset, color)
    if program == nil then
        program = self.program
    end
    if uv_scale_offset == nil then
        uv_scale_offset = { 1, 1, 0, 0 }
    end
    if color == nil then
        color = { 1, 1, 1, 1 }
    end

    local vbo = string.unpack("i", self.vbo)
    local ibo = string.unpack("i", self.ibo)

    -- draw
    gl.UseProgram(program)
    gl.UniformMatrix(program, "uMatrix", mvp)
    gl.Uniform4f(program, "uTextureCoordScaleOffset", uv_scale_offset[1], uv_scale_offset[2], uv_scale_offset[3], uv_scale_offset[4])
    gl.Uniform4f(program, "uColor", color[1], color[2], color[3], color[4])
    
    if texture then
        gl.UniformTexture(program, "uTexture0", 0, texture)
    end

    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
    gl.VertexAttrib(program, "aPosition", 2, 4 * 4, 0)
    gl.VertexAttrib(program, "aTextureCoord", 2, 4 * 4, 4 * 2)
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0)

    -- restore
    gl.DisableVertexAttribs(program)
end

return QuadRenderer
