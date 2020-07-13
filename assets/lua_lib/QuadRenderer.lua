local gl = require("gl")

return {
    New = function(self)
        local o = { }
        setmetatable(o, self)
        self.__index = self
        return o
    end,

    Init = function(self, context, effect)
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
            attribute vec2 aPosition;
            attribute vec2 aTextureCoord;
            varying vec2 vTextureCoord;
            void main()
            {
                gl_Position = uMatrix * vec4(aPosition, 0.0, 1.0);
                vTextureCoord = aTextureCoord;
            }
            ]]
        local fs = [[
            precision highp float;
            uniform sampler2D uTexture0;
            varying vec2 vTextureCoord;
            void main()
            {
                gl_FragColor = texture2D(uTexture0, vTextureCoord);
            }
            ]]

        self.program = gl.CreateProgram(context, vs, fs)
    end,

    Done = function(self, context, effect)
        if self.vbo then
            glDeleteBuffers(1, self.vbo)
        end

        if self.ibo then
            glDeleteBuffers(1, self.ibo)
        end

        if self.program then
            gl.DeleteProgram(self.program)
        end
    end,

    Render = function(self, context, effect, texture, mvp, program)
        if program == nil then
            program = self.program
        end

        local vbo = string.unpack("i", self.vbo)
        local ibo = string.unpack("i", self.ibo)

        -- draw
        gl.UseProgram(program)
        gl.UniformMatrix(program, "uMatrix", mvp)

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
    end,

    vbo = nil,
    ibo = nil,
    program = nil,
}
