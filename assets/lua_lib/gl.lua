--[[
    gl.GetError()
    gl.CreateProgram(context, vs, fs)
    gl.DestroyProgram(program)
    gl.UseProgram(program)
    gl.VertexAttrib(program, name, size, stride, offset)
    gl.DisableVertexAttribs(program)
    gl.Uniform1f(program, name, f)
    gl.Uniform2f(program, name, x, y)
    gl.Uniform3f(program, name, x, y, z)
    gl.Uniform4f(program, name, x, y, z, w)
    gl.UniformMatrix(program, name, matrix)
    gl.Uniform1i(program, name, i)
    gl.UniformTexture(program, name, index, texture)
    gl.AttachColorTexture(texture)
    gl.GetDepthRenderbufferFormat(context)
    gl.GetDepthTextureFormat(context)
    gl.GetRedTextureFormat(context)
    gl.Instance(context)
    gl.CreateTexture(target, format, width, height, filter_mode, wrap_mode)
    gl.LoadTexture(context, path, filter_mode, wrap_mode)
    gl.DestroyTexture(context, texture)
    gl.UpdateTexture(texture, x, y, w, h, data)
]]

local gl = { }

function gl.GetError()
    local err = glGetError()
    if err ~= 0 then
        LOGE("gl.GetError: " .. err .. "\n" .. debug.traceback())
    end
end

-- program
function gl.CreateProgram(context, vs, fs)
    local handle = LFX_BinaryString(4)
    local ret = LFX_Context_CreateProgram(context, vs, fs, handle)
    if ret ~= LFX_SUCCESS then
        return nil
    end

    local program = {
        id = 0,
        handle = handle,
        attributes = { },
        uniforms = { },
    }
    program.id = string.unpack("i", handle)

    -- attributes
    local int_handle = LFX_BinaryString(4)
    glGetProgramiv(program.id, GL_ACTIVE_ATTRIBUTES, int_handle)
    local attribute_count = string.unpack("i", int_handle)
    glGetProgramiv(program.id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, int_handle)
    local attribute_name_size = string.unpack("i", int_handle)
    for i = 1, attribute_count do
        local name = LFX_BinaryString(attribute_name_size)
        local length_handle = LFX_BinaryString(4)
        local size_handle = LFX_BinaryString(4)
        local type_handle = LFX_BinaryString(4)

        glGetActiveAttrib(program.id, i - 1, attribute_name_size, length_handle, size_handle, type_handle, name)

        local length = string.unpack("i", length_handle)
        local size = string.unpack("i", size_handle)
        local type = string.unpack("i", type_handle)
        name = string.sub(name, 1, length)
        local location = glGetAttribLocation(program.id, name)

        local attribute = {
            name = name,
            size = size,
            type = type,
            location = location,
        }
        program.attributes[name] = attribute
    end

    -- uniforms
    glGetProgramiv(program.id, GL_ACTIVE_UNIFORMS, int_handle)
    local uniform_count = string.unpack("i", int_handle)
    glGetProgramiv(program.id, GL_ACTIVE_UNIFORM_MAX_LENGTH, int_handle)
    local uniform_name_size = string.unpack("i", int_handle)
    for i = 1, uniform_count do
        local name = LFX_BinaryString(uniform_name_size)
        local length_handle = LFX_BinaryString(4)
        local size_handle = LFX_BinaryString(4)
        local type_handle = LFX_BinaryString(4)
            
        glGetActiveUniform(program.id, i - 1, uniform_name_size, length_handle, size_handle, type_handle, name)

        local length = string.unpack("i", length_handle)
        local size = string.unpack("i", size_handle)
        local type = string.unpack("i", type_handle)
        name = string.sub(name, 1, length)
        local location = glGetUniformLocation(program.id, name)

        local uniform = {
            name = name,
            size = size,
            type = type,
            location = location,
        }
        program.uniforms[name] = uniform
    end

    gl.GetError()

    return program
end

function gl.DestroyProgram(program)
    if program.id > 0 then
        glDeleteProgram(program.id)
        program.id = 0
    end

    gl.GetError()
end

function gl.UseProgram(program)
    glUseProgram(program.id)

    gl.GetError()
end

function gl.VertexAttrib(program, name, size, stride, offset)
    local attribute = program.attributes[name]
    if attribute then
        local loc = attribute.location
        glEnableVertexAttribArray(loc)
        glVertexAttribPointer(loc, size, GL_FLOAT, GL_FALSE, stride, offset)
    end

    gl.GetError()
end

function gl.DisableVertexAttribs(program)
    for name, attribute in pairs(program.attributes) do
        glDisableVertexAttribArray(attribute.location)
    end

    gl.GetError()
end

function gl.Uniform1f(program, name, f)
    glUniform1f(program.uniforms[name].location, f)
end

function gl.Uniform2f(program, name, x, y)
    glUniform2f(program.uniforms[name].location, x, y)

    gl.GetError()
end

function gl.Uniform3f(program, name, x, y, z)
    glUniform3f(program.uniforms[name].location, x, y, z)
end

function gl.Uniform4f(program, name, x, y, z, w)
    glUniform4f(program.uniforms[name].location, x, y, z, w)

    gl.GetError()
end

function gl.UniformMatrix(program, name, matrix)
    glUniformMatrix4fv(program.uniforms[name].location, 1, GL_FALSE, matrix)

    gl.GetError()
end

function gl.Uniform1i(program, name, i)
    glUniform1i(program.uniforms[name].location, i)

    gl.GetError()
end

function gl.UniformTexture(program, name, index, texture)
    glUniform1i(program.uniforms[name].location, index)
    glActiveTexture(GL_TEXTURE0 + index)
    glBindTexture(texture.target, texture.id)

    gl.GetError()
end

-- framebuffer
function gl.AttachColorTexture(texture)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.target, texture.id, 0)
    glViewport(0, 0, texture.width, texture.height)

    gl.GetError()
end

-- depth format
function gl.GetDepthRenderbufferFormat(context)
    local format = 0
    local major = LFX_BinaryString(4)
    local minor = LFX_BinaryString(4)
    local is_es = LFX_BinaryString(4)
    LFX_Context_GetGLVersion(context, major, minor, is_es)
    if string.unpack("i", major) >= 3 then
        format = GL_DEPTH_COMPONENT24
        LOGI("gl.GetDepthRenderbufferFormat: GL_DEPTH_COMPONENT24")
    else
        if LFX_Context_CheckGLExtension(context, "GL_OES_depth24") == LFX_SUCCESS then
            format = GL_DEPTH_COMPONENT24_OES
            LOGI("gl.GetDepthRenderbufferFormat: GL_DEPTH_COMPONENT24_OES")
        else
            format = GL_DEPTH_COMPONENT16
            LOGI("gl.GetDepthRenderbufferFormat: GL_DEPTH_COMPONENT16")
        end
    end

    gl.GetError()
        
    return format
end

function gl.GetDepthTextureFormat(context)
    local format = 0
    local major = LFX_BinaryString(4)
    local minor = LFX_BinaryString(4)
    local is_es = LFX_BinaryString(4)
    LFX_Context_GetGLVersion(context, major, minor, is_es)
    if string.unpack("i", major) >= 3 then
        format = GL_DEPTH_COMPONENT24
        LOGI("gl.GetDepthTextureFormat: GL_DEPTH_COMPONENT24")
    else
        if LFX_Context_CheckGLExtension(context, "GL_OES_depth_texture") == LFX_SUCCESS then
            format = GL_DEPTH_COMPONENT
            LOGI("gl.GetDepthTextureFormat: GL_DEPTH_COMPONENT")
        else
            LOGE("gl.GetDepthTextureFormat: not support depth texture")
        end
    end

    gl.GetError()

    return format
end

-- red format
function gl.GetRedTextureFormat(context)
    local format = 0
    local major = LFX_BinaryString(4)
    local minor = LFX_BinaryString(4)
    local is_es = LFX_BinaryString(4)
    LFX_Context_GetGLVersion(context, major, minor, is_es)
    if string.unpack("i", major) >= 3 then
        format = GL_R8
    else
        format = GL_LUMINANCE
    end

    gl.GetError()

    return format
end

-- instance
function gl.Instance(context)
    local instance = {
        is_support = false,
        support_instance = false,
        support_instance_ext = false,
        VertexAttribDivisor = nil,
        DrawElementsInstanced = nil,
    }
    local major = LFX_BinaryString(4)
    local minor = LFX_BinaryString(4)
    local is_es = LFX_BinaryString(4)
    LFX_Context_GetGLVersion(context, major, minor, is_es)
    local version = {
        major = string.unpack("i", major),
        minor = string.unpack("i", minor),
        is_es = string.unpack("i", is_es),
    }
    if version.is_es then
        if version.major >= 3 then
            instance.support_instance = true
            LOGI("gl.Instance: support instance")
        elseif version.major == 2 then
            if LFX_Context_CheckGLExtension(context, "GL_EXT_instanced_arrays") == LFX_SUCCESS and
                LFX_Context_CheckGLExtension(context, "GL_EXT_draw_instanced") == LFX_SUCCESS then
                instance.support_instance_ext = true
                LOGI("gl.Instance: support instance ext")
            end
        end
    else
        if (version.major == 3 and version.minor >= 3) or (version.major > 3) then
            instance.support_instance = true
            LOGI("gl.Instance: support instance")
        end
    end
    instance.is_support = instance.support_instance or instance.support_instance_ext
    if instance.support_instance then
        instance.VertexAttribDivisor = glVertexAttribDivisor
        instance.DrawElementsInstanced = glDrawElementsInstanced
    elseif instance.support_instance_ext then
        instance.VertexAttribDivisor = glVertexAttribDivisorEXT
        instance.DrawElementsInstanced = glDrawElementsInstancedEXT
    end

    gl.GetError()

    return instance
end

-- texture
function gl.CreateTexture(target, format, width, height, filter_mode, wrap_mode)
    local texel_format = 0
    local texel_type = 0
    if format == GL_ALPHA then
        texel_format = GL_ALPHA
        texel_type = GL_UNSIGNED_BYTE
    elseif format == GL_LUMINANCE then
        texel_format = GL_LUMINANCE
        texel_type = GL_UNSIGNED_BYTE
    elseif format == GL_R8 then
        texel_format = GL_RED
        texel_type = GL_UNSIGNED_BYTE
    elseif format == GL_RGBA then
        texel_format = GL_RGBA
        texel_type = GL_UNSIGNED_BYTE
    elseif format == GL_DEPTH_COMPONENT24 or format == GL_DEPTH_COMPONENT then
        texel_format = GL_DEPTH_COMPONENT
        texel_type = GL_UNSIGNED_INT
    else
        LOGE("gl.CreateTexture not support format: " .. format)
        return nil
    end
        
    local texture = {
        id = 0,
        target = target,
        format = format,
        width = width,
        height = height,
        filter_mode = filter_mode,
        wrap_mode = wrap_mode,
        tex = LFX_BinaryString(4),
        texel_format = texel_format,
        texel_type = texel_type,
    }
    glGenTextures(1, texture.tex)
    texture.id = string.unpack("i", texture.tex)

    glBindTexture(texture.target, texture.id)
    glTexImage2D(texture.target, 0, texture.format, texture.width, texture.height, 0, texel_format, texel_type, nil)
    glTexParameteri(texture.target, GL_TEXTURE_MIN_FILTER, texture.filter_mode)
    glTexParameteri(texture.target, GL_TEXTURE_MAG_FILTER, texture.filter_mode)
    glTexParameteri(texture.target, GL_TEXTURE_WRAP_S, texture.wrap_mode)
    glTexParameteri(texture.target, GL_TEXTURE_WRAP_T, texture.wrap_mode)
    glBindTexture(texture.target, 0)

    gl.GetError()

    return texture
end

function gl.LoadTexture(context, path, filter_mode, wrap_mode)
    local texture = {
        id = 0,
        target = 0,
        format = 0,
        width = 0,
        height = 0,
        filter_mode = filter_mode,
        wrap_mode = wrap_mode,
    }
    local ret = LFX_Context_LoadTexture2D(context, path, texture)
    if ret ~= LFX_SUCCESS then
        return nil
    end

    gl.GetError()

    return texture
end

function gl.DestroyTexture(context, texture)
    if texture.id > 0 then
        LFX_Context_DestroyTexture(context, texture)
        texture.id = 0
    end

    gl.GetError()
end

function gl.UpdateTexture(texture, x, y, w, h, data)
    glBindTexture(texture.target, texture.id)
    glTexSubImage2D(texture.target, 0, x, y, w, h, texture.texel_format, texture.texel_type, data)
    glBindTexture(texture.target, 0)

    gl.GetError()
end

return gl
