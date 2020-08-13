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
    self.program_instance = nil

    -- vbo
    local vertices = LFX_Float32ArrayCreateFromTable({
        -1, 1, 0,   0, 0,
        -1, -1, 0,  0, 1,
        1, -1, 0,   1, 1,
        1, 1, 0,    1, 0,
    })
    local indices = LFX_Uint16ArrayCreateFromTable({
        0, 1, 2, 0, 2, 3
    })

    self.vbo = LFX_BinaryString(4)
    glGenBuffers(1, self.vbo)
    local vbo = string.unpack("i", self.vbo)

    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBufferData(GL_ARRAY_BUFFER, 5 * 4 * 4, vertices, GL_STATIC_DRAW)
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
        attribute vec3 aPosition;
        attribute vec2 aTextureCoord;
        varying vec2 vTextureCoord;
        void main()
        {
            gl_Position = uMatrix * vec4(aPosition, 1.0);
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

    if self.program_instance then
        gl.DestroyProgram(self.program_instance)
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
    gl.UniformTexture(program, "uTexture0", 0, texture)
    gl.UniformMatrix(program, "uMatrix", mvp)
    gl.Uniform4f(program, "uTextureCoordScaleOffset", uv_scale_offset[1], uv_scale_offset[2], uv_scale_offset[3], uv_scale_offset[4])
    gl.Uniform4f(program, "uColor", color[1], color[2], color[3], color[4])

    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
    gl.VertexAttrib(program, "aPosition", 3, 5 * 4, 0)
    gl.VertexAttrib(program, "aTextureCoord", 2, 5 * 4, 3 * 4)
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0)

    -- restore
    gl.DisableVertexAttribs(program)
end

function QuadRenderer:RenderInstance(texture, program, instance_api, instance_vbo, instance_count)
    if program == nil then
        if self.program_instance == nil then
            local vs = [[
                attribute vec3 aPosition;
                attribute vec2 aTextureCoord;
                attribute vec4 aInstanceMVP_0;
                attribute vec4 aInstanceMVP_1;
                attribute vec4 aInstanceMVP_2;
                attribute vec4 aInstanceMVP_3;
                attribute vec4 aInstanceUVScaleOffset;
                attribute vec4 aInstanceColor;
                varying vec2 vTextureCoord;
                varying vec4 vColor;
                void main()
                {
                    mat4 mvp = mat4(aInstanceMVP_0, aInstanceMVP_1, aInstanceMVP_2, aInstanceMVP_3);
                    gl_Position = mvp * vec4(aPosition, 1.0);
                    vTextureCoord = aTextureCoord * aInstanceUVScaleOffset.xy + aInstanceUVScaleOffset.zw;
                    vColor = aInstanceColor;
                }
                ]]
            local fs = [[
                precision highp float;
                uniform sampler2D uTexture0;
                varying vec2 vTextureCoord;
                varying vec4 vColor;
                void main()
                {
                    gl_FragColor = texture2D(uTexture0, vTextureCoord) * vColor;
                }
                ]]

            self.program_instance = gl.CreateProgram(self.context, vs, fs)
        end
        program = self.program_instance
    end

    local vbo = string.unpack("i", self.vbo)
    local ibo = string.unpack("i", self.ibo)

    gl.UseProgram(program)
    gl.UniformTexture(program, "uTexture0", 0, texture)

    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
    gl.VertexAttrib(program, "aPosition", 3, 5 * 4, 0)
    gl.VertexAttrib(program, "aTextureCoord", 2, 5 * 4, 3 * 4)

    glBindBuffer(GL_ARRAY_BUFFER, instance_vbo)
    gl.VertexAttrib(program, "aInstanceMVP_0", 4, 24 * 4, 0)
    gl.VertexAttrib(program, "aInstanceMVP_1", 4, 24 * 4, 4 * 4)
    gl.VertexAttrib(program, "aInstanceMVP_2", 4, 24 * 4, 8 * 4)
    gl.VertexAttrib(program, "aInstanceMVP_3", 4, 24 * 4, 12 * 4)
    gl.VertexAttrib(program, "aInstanceUVScaleOffset", 4, 24 * 4, 16 * 4)
    gl.VertexAttrib(program, "aInstanceColor", 4, 24 * 4, 20 * 4)

    instance_api.VertexAttribDivisor(program.attributes["aInstanceMVP_0"].location, 1)
    instance_api.VertexAttribDivisor(program.attributes["aInstanceMVP_1"].location, 1)
    instance_api.VertexAttribDivisor(program.attributes["aInstanceMVP_2"].location, 1)
    instance_api.VertexAttribDivisor(program.attributes["aInstanceMVP_3"].location, 1)
    instance_api.VertexAttribDivisor(program.attributes["aInstanceUVScaleOffset"].location, 1)
    instance_api.VertexAttribDivisor(program.attributes["aInstanceColor"].location, 1)

    instance_api.DrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, instance_count)
    
    instance_api.VertexAttribDivisor(program.attributes["aInstanceMVP_0"].location, 0)
    instance_api.VertexAttribDivisor(program.attributes["aInstanceMVP_1"].location, 0)
    instance_api.VertexAttribDivisor(program.attributes["aInstanceMVP_2"].location, 0)
    instance_api.VertexAttribDivisor(program.attributes["aInstanceMVP_3"].location, 0)
    instance_api.VertexAttribDivisor(program.attributes["aInstanceUVScaleOffset"].location, 0)
    instance_api.VertexAttribDivisor(program.attributes["aInstanceColor"].location, 0)

    -- restore
    gl.DisableVertexAttribs(program)
end

return QuadRenderer
