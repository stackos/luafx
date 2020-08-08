local gl = require("gl")
local QuadRenderer = require("QuadRenderer")

local Canvas = { }

-- public
function Canvas.New()
    local o = { }
    setmetatable(o, Canvas)
    Canvas.__index = Canvas
    return o
end

function Canvas:Init(context, width, height)
    self.context = context
    self.width = width
    self.height = height

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
        uniform vec4 uColor;
        varying vec2 vTextureCoord;
        void main()
        {
            float a = texture2D(uTexture0, vTextureCoord).r;
            gl_FragColor = vec4(uColor.rgb, uColor.a * a);
        }
        ]]

    self.text_program = gl.CreateProgram(self.context, vs, fs)
end

function Canvas:Done()
    if self.text_program then
        gl.DeleteProgram(self.text_program)
    end
end

function Canvas:CreateText(str, font)
    local vertices = { }
    local indices = { }
    local rect = nil

    local pen = { 0, 0 }
    local codes = { }
    for pos, code in utf8.codes(str) do
        codes[#codes + 1] = code
    end
    for i = 1, #codes do
        local code = codes[i]
        local glyph = font:GetGlyph(code)

        if glyph.char == "\n" then
            pen[1] = 0
            pen[2] = pen[2] + (font.ascent - font.descent + font.line_gap) * font.font_scale
            pen[2] = math.floor(pen[2])
            goto continue
        end

        local left = pen[1] + glyph.bitmap_box[1]
        local top = pen[2] + glyph.bitmap_box[2]
        local right = left + glyph.bitmap_box[3] - glyph.bitmap_box[1]
        local bottom = top + glyph.bitmap_box[4] - glyph.bitmap_box[2]

        if rect == nil then
            rect = { left, top, right, bottom }
        else
            rect[1] = math.min(rect[1], left)
            rect[2] = math.min(rect[2], top)
            rect[3] = math.max(rect[3], right)
            rect[4] = math.max(rect[4], bottom)
        end

        local vertex_count = math.floor(#vertices / 4)
        vertices[#vertices + 1] = left
        vertices[#vertices + 1] = -top
        vertices[#vertices + 1] = glyph.texture_rect[1]
        vertices[#vertices + 1] = glyph.texture_rect[2]
        vertices[#vertices + 1] = left
        vertices[#vertices + 1] = -bottom
        vertices[#vertices + 1] = glyph.texture_rect[1]
        vertices[#vertices + 1] = glyph.texture_rect[4]
        vertices[#vertices + 1] = right
        vertices[#vertices + 1] = -bottom
        vertices[#vertices + 1] = glyph.texture_rect[3]
        vertices[#vertices + 1] = glyph.texture_rect[4]
        vertices[#vertices + 1] = right
        vertices[#vertices + 1] = -top
        vertices[#vertices + 1] = glyph.texture_rect[3]
        vertices[#vertices + 1] = glyph.texture_rect[2]
        indices[#indices + 1] = vertex_count + 0
        indices[#indices + 1] = vertex_count + 1
        indices[#indices + 1] = vertex_count + 2
        indices[#indices + 1] = vertex_count + 0
        indices[#indices + 1] = vertex_count + 2
        indices[#indices + 1] = vertex_count + 3

        pen[1] = pen[1] + glyph.advance * font.font_scale
        if i < #codes then
            pen[1] = pen[1] + font:GetGlyphKernAdvance(codes[i], codes[i + 1]) * font.font_scale
        end
        pen[1] = math.floor(pen[1])

        ::continue::
    end

    local vertex_buffer = LFX_Float32ArrayCreateFromTable(vertices)
    local index_buffer = LFX_Uint16ArrayCreateFromTable(indices)

    local pvbo = LFX_BinaryString(4)
    glGenBuffers(1, pvbo)
    local vbo = string.unpack("i", pvbo)

    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBufferData(GL_ARRAY_BUFFER, #vertices * 4, vertex_buffer, GL_STATIC_DRAW)
    glBindBuffer(GL_ARRAY_BUFFER, 0)

    local pibo = LFX_BinaryString(4)
    glGenBuffers(1, pibo)
    local ibo = string.unpack("i", pibo)

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, #indices * 2, index_buffer, GL_STATIC_DRAW)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)

    LFX_Free(vertex_buffer)
    LFX_Free(index_buffer)

    local text_mesh = {
        text = str,
        font = font,
        vertices = vertices,
        indices = indices,
        rect = rect,
        pvbo = pvbo,
        pibo = pibo,
        vbo = vbo,
        ibo = ibo,
    }

    return text_mesh
end

function Canvas:DestroyText(text_mesh)
    if text_mesh.pvbo then
        glDeleteBuffers(1, text_mesh.pvbo)
    end
    if text_mesh.pibo then
        glDeleteBuffers(1, text_mesh.pibo)
    end
end

function Canvas:DrawText(text_mesh, x, y, color)
    local font = text_mesh.font
    local index_count = #text_mesh.indices
    local vbo = text_mesh.vbo
    local ibo = text_mesh.ibo
    local program = self.text_program

    -- state
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    -- mvp
    local model = mat4()
    glm_translate_make(model, vec3(-self.width / 2 + x, self.height / 2 - y, 0))
    local view = mat4()
    glm_lookat_lh(vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 0), view)
    local proj = mat4()
    glm_ortho_lh(-self.width / 2, self.width / 2, -self.height / 2, self.height / 2, -1, 1, proj)
    local vp = mat4()
    glm_mat4_mul(proj, view, vp)
    glm_mat4_mul(LFX_MAT4_FLIP_Y, vp, vp)
    local mvp = mat4()
    glm_mat4_mul(vp, model, mvp)

    -- draw
    gl.UseProgram(program)
    gl.UniformMatrix(program, "uMatrix", mvp)
    gl.UniformTexture(program, "uTexture0", 0, font:GetTexture())
    gl.Uniform4f(program, "uColor", color[1], color[2], color[3], color[4])

    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
    gl.VertexAttrib(program, "aPosition", 2, 4 * 4, 0)
    gl.VertexAttrib(program, "aTextureCoord", 2, 4 * 4, 4 * 2)
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, 0)

    -- restore
    gl.DisableVertexAttribs(program)

    glDisable(GL_BLEND)
end

return Canvas
