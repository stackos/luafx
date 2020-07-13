return {
    CreateProgram = function(context, vs, fs)
        local handle = LFX_BinaryString(4)
        local ret = LFX_Context_CreateProgram(context, vs, fs, handle)
        if ret ~= LFX_SUCCESS then
            return nil
        end

        local program = {
            handle = handle,
            attributes = { },
            uniforms = { },
        }
        local p = string.unpack("i", handle)

        -- attributes
        local int_handle = LFX_BinaryString(4)
        glGetProgramiv(p, GL_ACTIVE_ATTRIBUTES, int_handle)
        local attribute_count = string.unpack("i", int_handle)
        glGetProgramiv(p, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, int_handle)
        local attribute_name_size = string.unpack("i", int_handle)
        for i = 1, attribute_count do
            local name = LFX_BinaryString(attribute_name_size)
            local length_handle = LFX_BinaryString(4)
            local size_handle = LFX_BinaryString(4)
            local type_handle = LFX_BinaryString(4)

            glGetActiveAttrib(p, i - 1, attribute_name_size, length_handle, size_handle, type_handle, name)

            local length = string.unpack("i", length_handle)
            local size = string.unpack("i", size_handle)
            local type = string.unpack("i", type_handle)
            name = string.sub(name, 1, length)
            local location = glGetAttribLocation(p, name)

            local attribute = {
                name = name,
                size = size,
                type = type,
                location = location,
            }
            program.attributes[name] = attribute
        end

        -- uniforms
        glGetProgramiv(p, GL_ACTIVE_UNIFORMS, int_handle)
        local uniform_count = string.unpack("i", int_handle)
        glGetProgramiv(p, GL_ACTIVE_UNIFORM_MAX_LENGTH, int_handle)
        local uniform_name_size = string.unpack("i", int_handle)
        for i = 1, uniform_count do
            local name = LFX_BinaryString(uniform_name_size)
            local length_handle = LFX_BinaryString(4)
            local size_handle = LFX_BinaryString(4)
            local type_handle = LFX_BinaryString(4)
            
            glGetActiveUniform(p, i - 1, uniform_name_size, length_handle, size_handle, type_handle, name)

            local length = string.unpack("i", length_handle)
            local size = string.unpack("i", size_handle)
            local type = string.unpack("i", type_handle)
            name = string.sub(name, 1, length)
            local location = glGetUniformLocation(p, name)

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
        local p = string.unpack("i", program.handle)
        glDeleteProgram(p)
    end,

    UseProgram = function(program)
        local p = string.unpack("i", program.handle)
        glUseProgram(p)
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
        local p = string.unpack("i", program.handle)
        glUniform1i(program.uniforms[name].location, index)
        glActiveTexture(GL_TEXTURE0 + index)
        glBindTexture(texture.target, texture.id)
    end,

    AttachColor = function(texture)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.target, texture.id, 0)
        glViewport(0, 0, texture.width, texture.height)
    end,
}
