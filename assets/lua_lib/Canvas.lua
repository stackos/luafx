local gl = require("gl")
local QuadRenderer = require("QuadRenderer")

local DrawCmd = {
    DrawText = 0,
    DrawTexture = 1,
}

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
    self.text_program = self:CreateTextProgram()
    self.quad_renderer = QuadRenderer.New()
    self.quad_renderer:Init(self.context)
    self.draw_call = 0
    self.draw_cmds = { }
    self.draw_batches = { }
    self.draw_vp = nil
    self.instance_api = gl.Instance(self.context)
    self.instance_buffer = nil
    self.instance_buffer_size = 0
    self.instance_vbo_ptr = nil
    self.instance_vbo = 0
    self.instance_vbo_size = 0

    self:SetSize(width, height)
end

function Canvas:Done()
    if self.instance_vbo_ptr then
        glDeleteBuffers(1, self.instance_vbo_ptr)
    end
    if self.instance_buffer then
        LFX_Free(self.instance_buffer)
    end
    self.quad_renderer:Done()
    if self.text_program then
        gl.DestroyProgram(self.text_program)
    end
end

function Canvas:SetSize(width, height)
    self.width = width
    self.height = height

    local view = mat4()
    glm_lookat_lh(vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 0), view)
    local proj = mat4()
    glm_ortho_lh(-self.width / 2, self.width / 2, -self.height / 2, self.height / 2, -1, 1, proj)
    local vp = mat4()
    glm_mat4_mul(proj, view, vp)
    glm_mat4_mul(LFX_MAT4_FLIP_Y, vp, vp)

    self.draw_vp = vp
end

function Canvas:CreateText(str, font, bold_text_mesh)
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
        local bitmap_box = glyph.bitmap_box
        local texture_rect = glyph.texture_rect
        
        if bold_text_mesh then
            texture_rect = glyph.bold_texture_rect
        end

        if glyph.char == "\n" then
            pen[1] = 0
            pen[2] = pen[2] + (font.ascent - font.descent + font.line_gap) * font.font_scale
            pen[2] = math.floor(pen[2])
            goto continue
        end

        local left = pen[1] + bitmap_box[1]
        local top = pen[2] + bitmap_box[2]
        local right = left + bitmap_box[3] - bitmap_box[1]
        local bottom = top + bitmap_box[4] - bitmap_box[2]

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
        vertices[#vertices + 1] = texture_rect[1]
        vertices[#vertices + 1] = texture_rect[2]
        vertices[#vertices + 1] = left
        vertices[#vertices + 1] = -bottom
        vertices[#vertices + 1] = texture_rect[1]
        vertices[#vertices + 1] = texture_rect[4]
        vertices[#vertices + 1] = right
        vertices[#vertices + 1] = -bottom
        vertices[#vertices + 1] = texture_rect[3]
        vertices[#vertices + 1] = texture_rect[4]
        vertices[#vertices + 1] = right
        vertices[#vertices + 1] = -top
        vertices[#vertices + 1] = texture_rect[3]
        vertices[#vertices + 1] = texture_rect[2]
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

    local text_mesh = {
        text = str,
        font = font,
        vertices = vertices,
        indices = indices,
        rect = rect,
        pvbo = nil,
        pibo = nil,
        vbo = nil,
        ibo = nil,
        bold_text_mesh = nil,
    }

    if #vertices > 0 and #indices > 0 then
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

        text_mesh.pvbo = pvbo
        text_mesh.pibo = pibo
        text_mesh.vbo = vbo
        text_mesh.ibo = ibo
    end

    if font.bold_size > 0 then
        if bold_text_mesh == nil or bold_text_mesh == false then
            text_mesh.bold_text_mesh = self:CreateText(str, font, true)
        end
    end

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

function Canvas:DrawBegin()
    self.draw_call = 0
    self.draw_cmds = { }
end

function Canvas:DrawText(text_mesh, x, y, color, outline_color)
    if #text_mesh.text == 0 then
        return
    end

    local cmd = {
        type = DrawCmd.DrawText,
        text_mesh = text_mesh,
        x = x,
        y = y,
        color = color,
    }
    
    if text_mesh.font.bold_size > 0 and outline_color then
        local offset = math.floor(text_mesh.font.bold_size / 2)
        local bold_cmd = {
            type = DrawCmd.DrawText,
            text_mesh = text_mesh.bold_text_mesh,
            x = x - offset,
            y = y + offset,
            color = outline_color,
        }

        if self.instance_api.is_support then
            self.draw_cmds[#self.draw_cmds + 1] = bold_cmd
        else
            self:DrawTextSingle(bold_cmd)
        end
    end

    if self.instance_api.is_support then
        self.draw_cmds[#self.draw_cmds + 1] = cmd
    else
        self:DrawTextSingle(cmd)
    end
end

function Canvas:DrawTexture(texture, x, y, w, h, sx, sy, sw, sh, deg, color)
    if texture == nil or x == nil or y == nil then
        return
    end

    if w == nil then
        w = texture.width
    end
    if h == nil then
        h = texture.height
    end
    if sx == nil then
        sx = 0
    end
    if sy == nil then
        sy = 0
    end
    if sw == nil then
        sw = texture.width
    end
    if sh == nil then
        sh = texture.height
    end
    if deg == nil then
        deg = 0
    end
    if color == nil then
        color = { 1, 1, 1, 1 }
    end

    local cmd = {
        type = DrawCmd.DrawTexture,
        texture = texture,
        x = x,
        y = y,
        w = w,
        h = h,
        sx = sx,
        sy = sy,
        sw = sw,
        sh = sh,
        deg = deg,
        color = color,
    }

    if self.instance_api.is_support then
        self.draw_cmds[#self.draw_cmds + 1] = cmd
    else
        self:DrawTextureSingle(cmd)
    end
end

function Canvas:DrawEnd()
    if #self.draw_cmds > 0 then
        self.draw_batches = { }
        local batch = { }
        for i = 1, #self.draw_cmds do
            local cmd = self.draw_cmds[i]
            if #batch == 0 then
                batch[#batch + 1] = cmd
            elseif batch[#batch].type == DrawCmd.DrawTexture and cmd.type == DrawCmd.DrawTexture and batch[#batch].texture == cmd.texture then
                batch[#batch + 1] = cmd
            else
                self.draw_batches[#self.draw_batches + 1] = batch
                batch = { cmd }
            end
        end
        self.draw_batches[#self.draw_batches + 1] = batch

        for i = 1, #self.draw_batches do
            local batch = self.draw_batches[i]
            if #batch == 1 then
                if batch[1].type == DrawCmd.DrawTexture then
                    self:DrawTextureSingle(batch[1])
                elseif batch[1].type == DrawCmd.DrawText then
                    self:DrawTextSingle(batch[1])
                end
            else
                self:DrawTextureBatch(batch)
            end
        end
    end
end

-- private
function Canvas:CreateTextProgram()
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

    return gl.CreateProgram(self.context, vs, fs)
end

function Canvas:DrawTextSingle(cmd)
    local text_mesh = cmd.text_mesh
    local x = cmd.x
    local y = cmd.y
    local color = cmd.color
    
    local font = text_mesh.font
    local index_count = #text_mesh.indices
    local vbo = text_mesh.vbo
    local ibo = text_mesh.ibo
    local program = self.text_program

    if font:GetTexture() == nil then
        return
    end

    -- state
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    -- mvp
    local model = mat4()
    glm_translate_make(model, vec3(-self.width / 2 + x, self.height / 2 - y, 0))
    local mvp = mat4()
    glm_mat4_mul(self.draw_vp, model, mvp)

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

    self.draw_call = self.draw_call + 1
end

function Canvas:GetTextureMVP(cmd)
    local scale = mat4()
    glm_scale_make(scale, vec3(0.5 * cmd.w, 0.5 * cmd.h, 1))
    local rotate = mat4()
    glm_euler_zxy(vec3(0, 0, glm_rad(cmd.deg)), rotate)
    local translate = mat4()
    glm_translate_make(translate, vec3(-self.width / 2 + cmd.x, self.height / 2 - cmd.y, 0))
    local model = mat4()
    glm_mat4_mul(rotate, scale, model)
    glm_mat4_mul(translate, model, model)

    local mvp = mat4()
    glm_mat4_mul(self.draw_vp, model, mvp)
    return mvp
end

function Canvas:DrawTextureSingle(cmd)
    -- mvp
    local mvp = self:GetTextureMVP(cmd)

    -- uv
    local uv_scale_offset = { 1, 1, 0, 0 }
    uv_scale_offset[1] = cmd.sw / cmd.texture.width
    uv_scale_offset[2] = cmd.sh / cmd.texture.height
    uv_scale_offset[3] = cmd.sx / cmd.texture.width
    uv_scale_offset[4] = cmd.sy / cmd.texture.height

    -- state
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    self.quad_renderer:Render(cmd.texture, mvp, nil, uv_scale_offset, cmd.color)

    -- restore
    glDisable(GL_BLEND)

    self.draw_call = self.draw_call + 1
end

function Canvas:DrawTextureBatch(batch)
    local instance_data = { }
    for i = 1, #batch do
        local cmd = batch[i]
        local mvp = self:GetTextureMVP(cmd)

        for j = 1, 16 do
            instance_data[#instance_data + 1] = LFX_Float32ArrayGetElement(mvp, j - 1)
        end
        
        local uv_scale_offset = { 1, 1, 0, 0 }
        uv_scale_offset[1] = cmd.sw / cmd.texture.width
        uv_scale_offset[2] = cmd.sh / cmd.texture.height
        uv_scale_offset[3] = cmd.sx / cmd.texture.width
        uv_scale_offset[4] = cmd.sy / cmd.texture.height

        for j = 1, 4 do
            instance_data[#instance_data + 1] = uv_scale_offset[j]
        end

        for j = 1, 4 do
            instance_data[#instance_data + 1] = cmd.color[j]
        end
    end
    
    local instance_buffer_size = #instance_data * 4
    if self.instance_buffer == nil or self.instance_buffer_size < instance_buffer_size then
        if self.instance_buffer then
            LFX_Free(self.instance_buffer)
            self.instance_buffer = nil
        end
        self.instance_buffer = LFX_Float32ArrayCreateFromTable(instance_data)
        self.instance_buffer_size = instance_buffer_size
    else
        LFX_Float32ArrayCopyFromTable(self.instance_buffer, 0, instance_data, 1, #instance_data)
    end

    if self.instance_vbo_ptr == nil then
        self.instance_vbo_ptr = LFX_BinaryString(4)
        glGenBuffers(1, self.instance_vbo_ptr)
        self.instance_vbo = string.unpack("i", self.instance_vbo_ptr)
    end
    
    glBindBuffer(GL_ARRAY_BUFFER, self.instance_vbo)
    if self.instance_vbo_size < self.instance_buffer_size then
        self.instance_vbo_size = self.instance_buffer_size
        glBufferData(GL_ARRAY_BUFFER, self.instance_vbo_size, self.instance_buffer, GL_DYNAMIC_DRAW)
    else
        glBufferSubData(GL_ARRAY_BUFFER, 0, self.instance_buffer_size, self.instance_buffer)
    end
    glBindBuffer(GL_ARRAY_BUFFER, 0)

    -- state
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    self.quad_renderer:RenderInstance(batch[1].texture, nil, self.instance_api, self.instance_vbo, #batch)

    -- restore
    glDisable(GL_BLEND)

    self.draw_call = self.draw_call + 1
end

return Canvas
