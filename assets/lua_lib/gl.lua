return {
    -- program
    CreateProgram = function(context, vs, fs)
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

        return program
    end,

    DeleteProgram = function(program)
        if program.id > 0 then
            glDeleteProgram(program.id)
            program.id = 0
        end
    end,

    UseProgram = function(program)
        glUseProgram(program.id)
    end,

    VertexAttrib = function(program, name, size, stride, offset)
        local attribute = program.attributes[name]
        if attribute then
            local loc = attribute.location
            glEnableVertexAttribArray(loc)
            glVertexAttribPointer(loc, size, GL_FLOAT, GL_FALSE, stride, offset)
        end
    end,

    DisableVertexAttribs = function(program)
        for name, attribute in pairs(program.attributes) do
            glDisableVertexAttribArray(attribute.location)
        end
    end,

    Uniform1f = function(program, name, f)
        glUniform1f(program.uniforms[name].location, f)
    end,

    Uniform2f = function(program, name, x, y)
        glUniform2f(program.uniforms[name].location, x, y)
    end,

    Uniform3f = function(program, name, x, y, z)
        glUniform3f(program.uniforms[name].location, x, y, z)
    end,

    Uniform4f = function(program, name, x, y, z, w)
        glUniform4f(program.uniforms[name].location, x, y, z, w)
    end,

    UniformMatrix = function(program, name, matrix)
        glUniformMatrix4fv(program.uniforms[name].location, 1, GL_FALSE, matrix)
    end,

    Uniform1i = function(program, name, i)
        glUniform1i(program.uniforms[name].location, i)
    end,

    UniformTexture = function(program, name, index, texture)
        glUniform1i(program.uniforms[name].location, index)
        glActiveTexture(GL_TEXTURE0 + index)
        glBindTexture(texture.target, texture.id)
    end,

    -- framebuffer
    AttachColorTexture = function(texture)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.target, texture.id, 0)
        glViewport(0, 0, texture.width, texture.height)
    end,

    -- depth format
    GetDepthRenderbufferFormat = function(context)
        local depth_format = 0
        local major = LFX_BinaryString(4)
        local minor = LFX_BinaryString(4)
        local is_es = LFX_BinaryString(4)
        LFX_Context_GetGLVersion(context, major, minor, is_es)
        if string.unpack("i", major) >= 3 then
            depth_format = GL_DEPTH_COMPONENT24
            LOGI("gl.GetDepthRenderbufferFormat: GL_DEPTH_COMPONENT24")
        else
            if LFX_Context_CheckGLExtension(context, "GL_OES_depth24") == LFX_SUCCESS then
                depth_format = GL_DEPTH_COMPONENT24_OES
                LOGI("gl.GetDepthRenderbufferFormat: GL_DEPTH_COMPONENT24_OES")
            else
                depth_format = GL_DEPTH_COMPONENT16
                LOGI("gl.GetDepthRenderbufferFormat: GL_DEPTH_COMPONENT16")
            end
        end
        return depth_format
    end,

    GetDepthTextureFormat = function(context)
        local depth_format = 0
        local major = LFX_BinaryString(4)
        local minor = LFX_BinaryString(4)
        local is_es = LFX_BinaryString(4)
        LFX_Context_GetGLVersion(context, major, minor, is_es)
        if string.unpack("i", major) >= 3 then
            depth_format = GL_DEPTH_COMPONENT24
            LOGI("gl.GetDepthTextureFormat: GL_DEPTH_COMPONENT24")
        else
            if LFX_Context_CheckGLExtension(context, "GL_OES_depth_texture") == LFX_SUCCESS then
                depth_format = GL_DEPTH_COMPONENT
                LOGI("gl.GetDepthTextureFormat: GL_DEPTH_COMPONENT")
            else
                LOGE("gl.GetDepthTextureFormat: not support depth texture")
            end
        end
        return depth_format
    end,

    -- instance
    Instance = function(context)
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
        return instance
    end,

    -- texture
    CreateTexture = function(target, format, width, height, filter_mode, wrap_mode)
        local texel_format = 0
        local texel_type = 0
        if format == GL_ALPHA then
            texel_format = GL_ALPHA
            texel_type = GL_UNSIGNED_BYTE
        elseif format == GL_LUMINANCE then
            texel_format = GL_LUMINANCE
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

        return texture
    end,

    DeleteTexture = function(texture)
        if texture.id > 0 then
            glDeleteTextures(1, texture.tex)
            texture.id = 0
        end
    end,

    UpdateTexture = function(texture, x, y, w, h, data)
        glBindTexture(texture.target, texture.id)
        glTexSubImage2D(texture.target, 0, x, y, w, h, texture.texel_format, texture.texel_type, data)
    end,
}
