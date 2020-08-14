#include "lua_bind_gl.h"
#include "lua_bind.h"
#include "lualib.h"
#include "lauxlib.h"
#include "gl_include.h"
#include <stdlib.h>

DEF_FUNC_V_I(glActiveTexture)
DEF_FUNC_V_II(glAttachShader)
DEF_FUNC_V_IIS(glBindAttribLocation)
DEF_FUNC_V_II(glBindBuffer)
DEF_FUNC_V_II(glBindFramebuffer)
DEF_FUNC_V_II(glBindRenderbuffer)
DEF_FUNC_V_II(glBindTexture)
DEF_FUNC_V_FFFF(glBlendColor)
DEF_FUNC_V_I(glBlendEquation)
DEF_FUNC_V_II(glBlendEquationSeparate)
DEF_FUNC_V_II(glBlendFunc)
DEF_FUNC_V_IIII(glBlendFuncSeparate)
DEF_FUNC_V_IIAI(glBufferData)
DEF_FUNC_V_IIIA(glBufferSubData)
DEF_FUNC_I_I(glCheckFramebufferStatus)
DEF_FUNC_V_I(glClear)
DEF_FUNC_V_FFFF(glClearColor)
DEF_FUNC_V_F(glClearDepthf) // require GL_ARB_ES2_compatibility or 4.1 for desktop
DEF_FUNC_V_I(glClearStencil)
DEF_FUNC_V_IIII(glColorMask)
DEF_FUNC_V_I(glCompileShader)
DEF_FUNC_V_IIIIIIIA(glCompressedTexImage2D)
DEF_FUNC_V_IIIIIIIIA(glCompressedTexSubImage2D)
DEF_FUNC_V_IIIIIIII(glCopyTexImage2D)
DEF_FUNC_V_IIIIIIII(glCopyTexSubImage2D)
DEF_FUNC_I_V(glCreateProgram)
DEF_FUNC_I_I(glCreateShader)
DEF_FUNC_V_I(glCullFace)
DEF_FUNC_V_IA(glDeleteBuffers)
DEF_FUNC_V_IA(glDeleteFramebuffers)
DEF_FUNC_V_I(glDeleteProgram)
DEF_FUNC_V_IA(glDeleteRenderbuffers)
DEF_FUNC_V_I(glDeleteShader)
DEF_FUNC_V_IA(glDeleteTextures)
DEF_FUNC_V_I(glDepthFunc)
DEF_FUNC_V_I(glDepthMask)
DEF_FUNC_V_FF(glDepthRangef) // require GL_ARB_ES2_compatibility or 4.1 for desktop
DEF_FUNC_V_II(glDetachShader)
DEF_FUNC_V_I(glDisable)
DEF_FUNC_V_I(glDisableVertexAttribArray)
DEF_FUNC_V_III(glDrawArrays)
DEF_FUNC_V_IIIA(glDrawElements)
DEF_FUNC_V_I(glEnable)
DEF_FUNC_V_I(glEnableVertexAttribArray)
DEF_FUNC_V_V(glFinish)
DEF_FUNC_V_V(glFlush)
DEF_FUNC_V_IIII(glFramebufferRenderbuffer)
DEF_FUNC_V_IIIII(glFramebufferTexture2D)
DEF_FUNC_V_I(glFrontFace)
DEF_FUNC_V_IA(glGenBuffers)
DEF_FUNC_V_I(glGenerateMipmap)
DEF_FUNC_V_IA(glGenFramebuffers)
DEF_FUNC_V_IA(glGenRenderbuffers)
DEF_FUNC_V_IA(glGenTextures)
DEF_FUNC_V_IIIAAAA(glGetActiveAttrib)
DEF_FUNC_V_IIIAAAA(glGetActiveUniform)
DEF_FUNC_V_IIAA(glGetAttachedShaders)
DEF_FUNC_I_IS(glGetAttribLocation)
DEF_FUNC_V_IA(glGetBooleanv)
DEF_FUNC_V_IIA(glGetBufferParameteriv)
DEF_FUNC_I_V(glGetError)
DEF_FUNC_V_IA(glGetFloatv)
DEF_FUNC_V_IIIA(glGetFramebufferAttachmentParameteriv)
DEF_FUNC_V_IA(glGetIntegerv)
DEF_FUNC_V_IIA(glGetProgramiv)
DEF_FUNC_V_IIAA(glGetProgramInfoLog)
DEF_FUNC_V_IIA(glGetRenderbufferParameteriv)
DEF_FUNC_V_IIA(glGetShaderiv)
DEF_FUNC_V_IIAA(glGetShaderInfoLog)
DEF_FUNC_V_IIAA(glGetShaderPrecisionFormat) // require GL_ARB_ES2_compatibility or 4.1 for desktop
DEF_FUNC_V_IIAA(glGetShaderSource)
DEF_FUNC_S_I(glGetString)
DEF_FUNC_V_IIA(glGetTexParameterfv)
DEF_FUNC_V_IIA(glGetTexParameteriv)
DEF_FUNC_V_IIA(glGetUniformfv)
DEF_FUNC_V_IIA(glGetUniformiv)
DEF_FUNC_I_IS(glGetUniformLocation)
DEF_FUNC_V_IIA(glGetVertexAttribfv)
DEF_FUNC_V_IIA(glGetVertexAttribiv)
DEF_FUNC_V_IIA(glGetVertexAttribPointerv)
DEF_FUNC_V_II(glHint)
DEF_FUNC_I_I(glIsBuffer)
DEF_FUNC_I_I(glIsEnabled)
DEF_FUNC_I_I(glIsFramebuffer)
DEF_FUNC_I_I(glIsProgram)
DEF_FUNC_I_I(glIsRenderbuffer)
DEF_FUNC_I_I(glIsShader)
DEF_FUNC_I_I(glIsTexture)
DEF_FUNC_V_F(glLineWidth)
DEF_FUNC_V_I(glLinkProgram)
DEF_FUNC_V_II(glPixelStorei)
DEF_FUNC_V_FF(glPolygonOffset)
DEF_FUNC_V_IIIIIIA(glReadPixels)
DEF_FUNC_V_V(glReleaseShaderCompiler) // require GL_ARB_ES2_compatibility or 4.1 for desktop
DEF_FUNC_V_IIII(glRenderbufferStorage)
DEF_FUNC_V_FI(glSampleCoverage)
DEF_FUNC_V_IIII(glScissor)
DEF_FUNC_V_IAIAI(glShaderBinary) // require GL_ARB_ES2_compatibility or 4.1 for desktop
DEF_FUNC_V_IIAA(glShaderSource)
DEF_FUNC_V_III(glStencilFunc)
DEF_FUNC_V_IIII(glStencilFuncSeparate)
DEF_FUNC_V_I(glStencilMask)
DEF_FUNC_V_II(glStencilMaskSeparate)
DEF_FUNC_V_III(glStencilOp)
DEF_FUNC_V_IIII(glStencilOpSeparate)
DEF_FUNC_V_IIIIIIIIA(glTexImage2D)
DEF_FUNC_V_IIF(glTexParameterf)
DEF_FUNC_V_IIA(glTexParameterfv)
DEF_FUNC_V_III(glTexParameteri)
DEF_FUNC_V_IIA(glTexParameteriv)
DEF_FUNC_V_IIIIIIIIA(glTexSubImage2D)
DEF_FUNC_V_IF(glUniform1f)
DEF_FUNC_V_IIA(glUniform1fv)
DEF_FUNC_V_II(glUniform1i)
DEF_FUNC_V_IIA(glUniform1iv)
DEF_FUNC_V_IFF(glUniform2f)
DEF_FUNC_V_IIA(glUniform2fv)
DEF_FUNC_V_III(glUniform2i)
DEF_FUNC_V_IIA(glUniform2iv)
DEF_FUNC_V_IFFF(glUniform3f)
DEF_FUNC_V_IIA(glUniform3fv)
DEF_FUNC_V_IIII(glUniform3i)
DEF_FUNC_V_IIA(glUniform3iv)
DEF_FUNC_V_IFFFF(glUniform4f)
DEF_FUNC_V_IIA(glUniform4fv)
DEF_FUNC_V_IIIII(glUniform4i)
DEF_FUNC_V_IIA(glUniform4iv)
DEF_FUNC_V_IIIA(glUniformMatrix2fv)
DEF_FUNC_V_IIIA(glUniformMatrix3fv)
DEF_FUNC_V_IIIA(glUniformMatrix4fv)
DEF_FUNC_V_I(glUseProgram)
DEF_FUNC_V_I(glValidateProgram)
DEF_FUNC_V_IF(glVertexAttrib1f)
DEF_FUNC_V_IA(glVertexAttrib1fv)
DEF_FUNC_V_IFF(glVertexAttrib2f)
DEF_FUNC_V_IA(glVertexAttrib2fv)
DEF_FUNC_V_IFFF(glVertexAttrib3f)
DEF_FUNC_V_IA(glVertexAttrib3fv)
DEF_FUNC_V_IFFFF(glVertexAttrib4f)
DEF_FUNC_V_IA(glVertexAttrib4fv)
DEF_FUNC_V_IIIIIA(glVertexAttribPointer)
DEF_FUNC_V_IIII(glViewport)

// ext
DEF_FUNC_V_II(glVertexAttribDivisorEXT);
DEF_FUNC_V_IIII(glDrawArraysInstancedEXT);
DEF_FUNC_V_IIIAI(glDrawElementsInstancedEXT);

// es3
DEF_FUNC_V_II(glVertexAttribDivisor);
DEF_FUNC_V_IIII(glDrawArraysInstanced);
DEF_FUNC_V_IIIAI(glDrawElementsInstanced);
DEF_FUNC_A_IIII(glMapBufferRange);
DEF_FUNC_V_I(glUnmapBuffer);
DEF_FUNC_V_III(glFlushMappedBufferRange);

static const luaL_Reg gles2_funcs[] = {
    REG_FUNC(glActiveTexture),
    REG_FUNC(glAttachShader),
    REG_FUNC(glBindAttribLocation),
    REG_FUNC(glBindBuffer),
    REG_FUNC(glBindFramebuffer),
    REG_FUNC(glBindRenderbuffer),
    REG_FUNC(glBindTexture),
    REG_FUNC(glBlendColor),
    REG_FUNC(glBlendEquation),
    REG_FUNC(glBlendEquationSeparate),
    REG_FUNC(glBlendFunc),
    REG_FUNC(glBlendFuncSeparate),
    REG_FUNC(glBufferData),
    REG_FUNC(glBufferSubData),
    REG_FUNC(glCheckFramebufferStatus),
    REG_FUNC(glClear),
    REG_FUNC(glClearColor),
    REG_FUNC(glClearDepthf), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_FUNC(glClearStencil),
    REG_FUNC(glColorMask),
    REG_FUNC(glCompileShader),
    REG_FUNC(glCompressedTexImage2D),
    REG_FUNC(glCompressedTexSubImage2D),
    REG_FUNC(glCopyTexImage2D),
    REG_FUNC(glCopyTexSubImage2D),
    REG_FUNC(glCreateProgram),
    REG_FUNC(glCreateShader),
    REG_FUNC(glCullFace),
    REG_FUNC(glDeleteBuffers),
    REG_FUNC(glDeleteFramebuffers),
    REG_FUNC(glDeleteProgram),
    REG_FUNC(glDeleteRenderbuffers),
    REG_FUNC(glDeleteShader),
    REG_FUNC(glDeleteTextures),
    REG_FUNC(glDepthFunc),
    REG_FUNC(glDepthMask),
    REG_FUNC(glDepthRangef), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_FUNC(glDetachShader),
    REG_FUNC(glDisable),
    REG_FUNC(glDisableVertexAttribArray),
    REG_FUNC(glDrawArrays),
    REG_FUNC(glDrawElements),
    REG_FUNC(glEnable),
    REG_FUNC(glEnableVertexAttribArray),
    REG_FUNC(glFinish),
    REG_FUNC(glFlush),
    REG_FUNC(glFramebufferRenderbuffer),
    REG_FUNC(glFramebufferTexture2D),
    REG_FUNC(glFrontFace),
    REG_FUNC(glGenBuffers),
    REG_FUNC(glGenerateMipmap),
    REG_FUNC(glGenFramebuffers),
    REG_FUNC(glGenRenderbuffers),
    REG_FUNC(glGenTextures),
    REG_FUNC(glGetActiveAttrib),
    REG_FUNC(glGetActiveUniform),
    REG_FUNC(glGetAttachedShaders),
    REG_FUNC(glGetAttribLocation),
    REG_FUNC(glGetBooleanv),
    REG_FUNC(glGetBufferParameteriv),
    REG_FUNC(glGetError),
    REG_FUNC(glGetFloatv),
    REG_FUNC(glGetFramebufferAttachmentParameteriv),
    REG_FUNC(glGetIntegerv),
    REG_FUNC(glGetProgramiv),
    REG_FUNC(glGetProgramInfoLog),
    REG_FUNC(glGetRenderbufferParameteriv),
    REG_FUNC(glGetShaderiv),
    REG_FUNC(glGetShaderInfoLog),
    REG_FUNC(glGetShaderPrecisionFormat), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_FUNC(glGetShaderSource),
    REG_FUNC(glGetString),
    REG_FUNC(glGetTexParameterfv),
    REG_FUNC(glGetTexParameteriv),
    REG_FUNC(glGetUniformfv),
    REG_FUNC(glGetUniformiv),
    REG_FUNC(glGetUniformLocation),
    REG_FUNC(glGetVertexAttribfv),
    REG_FUNC(glGetVertexAttribiv),
    REG_FUNC(glGetVertexAttribPointerv),
    REG_FUNC(glHint),
    REG_FUNC(glIsBuffer),
    REG_FUNC(glIsEnabled),
    REG_FUNC(glIsFramebuffer),
    REG_FUNC(glIsProgram),
    REG_FUNC(glIsRenderbuffer),
    REG_FUNC(glIsShader),
    REG_FUNC(glIsTexture),
    REG_FUNC(glLineWidth),
    REG_FUNC(glLinkProgram),
    REG_FUNC(glPixelStorei),
    REG_FUNC(glPolygonOffset),
    REG_FUNC(glReadPixels),
    REG_FUNC(glReleaseShaderCompiler), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_FUNC(glRenderbufferStorage),
    REG_FUNC(glSampleCoverage),
    REG_FUNC(glScissor),
    REG_FUNC(glShaderBinary), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_FUNC(glShaderSource),
    REG_FUNC(glStencilFunc),
    REG_FUNC(glStencilFuncSeparate),
    REG_FUNC(glStencilMask),
    REG_FUNC(glStencilMaskSeparate),
    REG_FUNC(glStencilOp),
    REG_FUNC(glStencilOpSeparate),
    REG_FUNC(glTexImage2D),
    REG_FUNC(glTexParameterf),
    REG_FUNC(glTexParameterfv),
    REG_FUNC(glTexParameteri),
    REG_FUNC(glTexParameteriv),
    REG_FUNC(glTexSubImage2D),
    REG_FUNC(glUniform1f),
    REG_FUNC(glUniform1fv),
    REG_FUNC(glUniform1i),
    REG_FUNC(glUniform1iv),
    REG_FUNC(glUniform2f),
    REG_FUNC(glUniform2fv),
    REG_FUNC(glUniform2i),
    REG_FUNC(glUniform2iv),
    REG_FUNC(glUniform3f),
    REG_FUNC(glUniform3fv),
    REG_FUNC(glUniform3i),
    REG_FUNC(glUniform3iv),
    REG_FUNC(glUniform4f),
    REG_FUNC(glUniform4fv),
    REG_FUNC(glUniform4i),
    REG_FUNC(glUniform4iv),
    REG_FUNC(glUniformMatrix2fv),
    REG_FUNC(glUniformMatrix3fv),
    REG_FUNC(glUniformMatrix4fv),
    REG_FUNC(glUseProgram),
    REG_FUNC(glValidateProgram),
    REG_FUNC(glVertexAttrib1f),
    REG_FUNC(glVertexAttrib1fv),
    REG_FUNC(glVertexAttrib2f),
    REG_FUNC(glVertexAttrib2fv),
    REG_FUNC(glVertexAttrib3f),
    REG_FUNC(glVertexAttrib3fv),
    REG_FUNC(glVertexAttrib4f),
    REG_FUNC(glVertexAttrib4fv),
    REG_FUNC(glVertexAttribPointer),
    REG_FUNC(glViewport),

    // ext
    REG_FUNC(glVertexAttribDivisorEXT), // ext GL_EXT_instanced_arrays
    REG_FUNC(glDrawArraysInstancedEXT), // ext GL_EXT_instanced_arrays
    REG_FUNC(glDrawElementsInstancedEXT), // ext GL_EXT_instanced_arrays

    { NULL, NULL }
};

static const luaL_Reg gles3_funcs[] = {
    REG_FUNC(glVertexAttribDivisor),
    REG_FUNC(glDrawArraysInstanced),
    REG_FUNC(glDrawElementsInstanced),
    REG_FUNC(glMapBufferRange),
    REG_FUNC(glUnmapBuffer),
    REG_FUNC(glFlushMappedBufferRange),

    { NULL, NULL }
};

static const DefineReg gles2_defines[] = {
    REG_DEF(GL_DEPTH_BUFFER_BIT),
    REG_DEF(GL_STENCIL_BUFFER_BIT),
    REG_DEF(GL_COLOR_BUFFER_BIT),
    REG_DEF(GL_FALSE),
    REG_DEF(GL_TRUE),
    REG_DEF(GL_POINTS),
    REG_DEF(GL_LINES),
    REG_DEF(GL_LINE_LOOP),
    REG_DEF(GL_LINE_STRIP),
    REG_DEF(GL_TRIANGLES),
    REG_DEF(GL_TRIANGLE_STRIP),
    REG_DEF(GL_TRIANGLE_FAN),
    REG_DEF(GL_ZERO),
    REG_DEF(GL_ONE),
    REG_DEF(GL_SRC_COLOR),
    REG_DEF(GL_ONE_MINUS_SRC_COLOR),
    REG_DEF(GL_SRC_ALPHA),
    REG_DEF(GL_ONE_MINUS_SRC_ALPHA),
    REG_DEF(GL_DST_ALPHA),
    REG_DEF(GL_ONE_MINUS_DST_ALPHA),
    REG_DEF(GL_DST_COLOR),
    REG_DEF(GL_ONE_MINUS_DST_COLOR),
    REG_DEF(GL_SRC_ALPHA_SATURATE),
    REG_DEF(GL_FUNC_ADD),
    REG_DEF(GL_BLEND_EQUATION),
    REG_DEF(GL_BLEND_EQUATION_RGB),
    REG_DEF(GL_BLEND_EQUATION_ALPHA),
    REG_DEF(GL_FUNC_SUBTRACT),
    REG_DEF(GL_FUNC_REVERSE_SUBTRACT),
    REG_DEF(GL_BLEND_DST_RGB),
    REG_DEF(GL_BLEND_SRC_RGB),
    REG_DEF(GL_BLEND_DST_ALPHA),
    REG_DEF(GL_BLEND_SRC_ALPHA),
    REG_DEF(GL_CONSTANT_COLOR),
    REG_DEF(GL_ONE_MINUS_CONSTANT_COLOR),
    REG_DEF(GL_CONSTANT_ALPHA),
    REG_DEF(GL_ONE_MINUS_CONSTANT_ALPHA),
    REG_DEF(GL_BLEND_COLOR),
    REG_DEF(GL_ARRAY_BUFFER),
    REG_DEF(GL_ELEMENT_ARRAY_BUFFER),
    REG_DEF(GL_ARRAY_BUFFER_BINDING),
    REG_DEF(GL_ELEMENT_ARRAY_BUFFER_BINDING),
    REG_DEF(GL_STREAM_DRAW),
    REG_DEF(GL_STATIC_DRAW),
    REG_DEF(GL_DYNAMIC_DRAW),
    REG_DEF(GL_BUFFER_SIZE),
    REG_DEF(GL_BUFFER_USAGE),
    REG_DEF(GL_CURRENT_VERTEX_ATTRIB),
    REG_DEF(GL_FRONT),
    REG_DEF(GL_BACK),
    REG_DEF(GL_FRONT_AND_BACK),
    REG_DEF(GL_TEXTURE_2D),
    REG_DEF(GL_CULL_FACE),
    REG_DEF(GL_BLEND),
    REG_DEF(GL_DITHER),
    REG_DEF(GL_STENCIL_TEST),
    REG_DEF(GL_DEPTH_TEST),
    REG_DEF(GL_SCISSOR_TEST),
    REG_DEF(GL_POLYGON_OFFSET_FILL),
    REG_DEF(GL_SAMPLE_ALPHA_TO_COVERAGE),
    REG_DEF(GL_SAMPLE_COVERAGE),
    REG_DEF(GL_NO_ERROR),
    REG_DEF(GL_INVALID_ENUM),
    REG_DEF(GL_INVALID_VALUE),
    REG_DEF(GL_INVALID_OPERATION),
    REG_DEF(GL_OUT_OF_MEMORY),
    REG_DEF(GL_CW),
    REG_DEF(GL_CCW),
    REG_DEF(GL_LINE_WIDTH),
    REG_DEF(GL_ALIASED_POINT_SIZE_RANGE),
    REG_DEF(GL_ALIASED_LINE_WIDTH_RANGE),
    REG_DEF(GL_CULL_FACE_MODE),
    REG_DEF(GL_FRONT_FACE),
    REG_DEF(GL_DEPTH_RANGE),
    REG_DEF(GL_DEPTH_WRITEMASK),
    REG_DEF(GL_DEPTH_CLEAR_VALUE),
    REG_DEF(GL_DEPTH_FUNC),
    REG_DEF(GL_STENCIL_CLEAR_VALUE),
    REG_DEF(GL_STENCIL_FUNC),
    REG_DEF(GL_STENCIL_FAIL),
    REG_DEF(GL_STENCIL_PASS_DEPTH_FAIL),
    REG_DEF(GL_STENCIL_PASS_DEPTH_PASS),
    REG_DEF(GL_STENCIL_REF),
    REG_DEF(GL_STENCIL_VALUE_MASK),
    REG_DEF(GL_STENCIL_WRITEMASK),
    REG_DEF(GL_STENCIL_BACK_FUNC),
    REG_DEF(GL_STENCIL_BACK_FAIL),
    REG_DEF(GL_STENCIL_BACK_PASS_DEPTH_FAIL),
    REG_DEF(GL_STENCIL_BACK_PASS_DEPTH_PASS),
    REG_DEF(GL_STENCIL_BACK_REF),
    REG_DEF(GL_STENCIL_BACK_VALUE_MASK),
    REG_DEF(GL_STENCIL_BACK_WRITEMASK),
    REG_DEF(GL_VIEWPORT),
    REG_DEF(GL_SCISSOR_BOX),
    REG_DEF(GL_COLOR_CLEAR_VALUE),
    REG_DEF(GL_COLOR_WRITEMASK),
    REG_DEF(GL_UNPACK_ALIGNMENT),
    REG_DEF(GL_PACK_ALIGNMENT),
    REG_DEF(GL_MAX_TEXTURE_SIZE),
    REG_DEF(GL_MAX_VIEWPORT_DIMS),
    REG_DEF(GL_SUBPIXEL_BITS),
    REG_DEF(GL_RED_BITS),
    REG_DEF(GL_GREEN_BITS),
    REG_DEF(GL_BLUE_BITS),
    REG_DEF(GL_ALPHA_BITS),
    REG_DEF(GL_DEPTH_BITS),
    REG_DEF(GL_STENCIL_BITS),
    REG_DEF(GL_POLYGON_OFFSET_UNITS),
    REG_DEF(GL_POLYGON_OFFSET_FACTOR),
    REG_DEF(GL_TEXTURE_BINDING_2D),
    REG_DEF(GL_SAMPLE_BUFFERS),
    REG_DEF(GL_SAMPLES),
    REG_DEF(GL_SAMPLE_COVERAGE_VALUE),
    REG_DEF(GL_SAMPLE_COVERAGE_INVERT),
    REG_DEF(GL_NUM_COMPRESSED_TEXTURE_FORMATS),
    REG_DEF(GL_COMPRESSED_TEXTURE_FORMATS),
    REG_DEF(GL_DONT_CARE),
    REG_DEF(GL_FASTEST),
    REG_DEF(GL_NICEST),
    REG_DEF(GL_GENERATE_MIPMAP_HINT),
    REG_DEF(GL_BYTE),
    REG_DEF(GL_UNSIGNED_BYTE),
    REG_DEF(GL_SHORT),
    REG_DEF(GL_UNSIGNED_SHORT),
    REG_DEF(GL_INT),
    REG_DEF(GL_UNSIGNED_INT),
    REG_DEF(GL_FLOAT),
    REG_DEF(GL_FIXED), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_DEPTH_COMPONENT),
    REG_DEF(GL_ALPHA),
    REG_DEF(GL_RGB),
    REG_DEF(GL_RGBA),
    REG_DEF(GL_LUMINANCE),
    REG_DEF(GL_LUMINANCE_ALPHA),
    REG_DEF(GL_UNSIGNED_SHORT_4_4_4_4),
    REG_DEF(GL_UNSIGNED_SHORT_5_5_5_1),
    REG_DEF(GL_UNSIGNED_SHORT_5_6_5),
    REG_DEF(GL_FRAGMENT_SHADER),
    REG_DEF(GL_VERTEX_SHADER),
    REG_DEF(GL_MAX_VERTEX_ATTRIBS),
    REG_DEF(GL_MAX_VERTEX_UNIFORM_VECTORS), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_MAX_VARYING_VECTORS), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS),
    REG_DEF(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS),
    REG_DEF(GL_MAX_TEXTURE_IMAGE_UNITS),
    REG_DEF(GL_MAX_FRAGMENT_UNIFORM_VECTORS), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_SHADER_TYPE),
    REG_DEF(GL_DELETE_STATUS),
    REG_DEF(GL_LINK_STATUS),
    REG_DEF(GL_VALIDATE_STATUS),
    REG_DEF(GL_ATTACHED_SHADERS),
    REG_DEF(GL_ACTIVE_UNIFORMS),
    REG_DEF(GL_ACTIVE_UNIFORM_MAX_LENGTH),
    REG_DEF(GL_ACTIVE_ATTRIBUTES),
    REG_DEF(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH),
    REG_DEF(GL_SHADING_LANGUAGE_VERSION),
    REG_DEF(GL_CURRENT_PROGRAM),
    REG_DEF(GL_NEVER),
    REG_DEF(GL_LESS),
    REG_DEF(GL_EQUAL),
    REG_DEF(GL_LEQUAL),
    REG_DEF(GL_GREATER),
    REG_DEF(GL_NOTEQUAL),
    REG_DEF(GL_GEQUAL),
    REG_DEF(GL_ALWAYS),
    REG_DEF(GL_KEEP),
    REG_DEF(GL_REPLACE),
    REG_DEF(GL_INCR),
    REG_DEF(GL_DECR),
    REG_DEF(GL_INVERT),
    REG_DEF(GL_INCR_WRAP),
    REG_DEF(GL_DECR_WRAP),
    REG_DEF(GL_VENDOR),
    REG_DEF(GL_RENDERER),
    REG_DEF(GL_VERSION),
    REG_DEF(GL_EXTENSIONS),
    REG_DEF(GL_NEAREST),
    REG_DEF(GL_LINEAR),
    REG_DEF(GL_NEAREST_MIPMAP_NEAREST),
    REG_DEF(GL_LINEAR_MIPMAP_NEAREST),
    REG_DEF(GL_NEAREST_MIPMAP_LINEAR),
    REG_DEF(GL_LINEAR_MIPMAP_LINEAR),
    REG_DEF(GL_TEXTURE_MAG_FILTER),
    REG_DEF(GL_TEXTURE_MIN_FILTER),
    REG_DEF(GL_TEXTURE_WRAP_S),
    REG_DEF(GL_TEXTURE_WRAP_T),
    REG_DEF(GL_TEXTURE),
    REG_DEF(GL_TEXTURE_CUBE_MAP),
    REG_DEF(GL_TEXTURE_BINDING_CUBE_MAP),
    REG_DEF(GL_TEXTURE_CUBE_MAP_POSITIVE_X),
    REG_DEF(GL_TEXTURE_CUBE_MAP_NEGATIVE_X),
    REG_DEF(GL_TEXTURE_CUBE_MAP_POSITIVE_Y),
    REG_DEF(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y),
    REG_DEF(GL_TEXTURE_CUBE_MAP_POSITIVE_Z),
    REG_DEF(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z),
    REG_DEF(GL_MAX_CUBE_MAP_TEXTURE_SIZE),
    REG_DEF(GL_TEXTURE0),
    REG_DEF(GL_TEXTURE1),
    REG_DEF(GL_TEXTURE2),
    REG_DEF(GL_TEXTURE3),
    REG_DEF(GL_TEXTURE4),
    REG_DEF(GL_TEXTURE5),
    REG_DEF(GL_TEXTURE6),
    REG_DEF(GL_TEXTURE7),
    REG_DEF(GL_TEXTURE8),
    REG_DEF(GL_TEXTURE9),
    REG_DEF(GL_TEXTURE10),
    REG_DEF(GL_TEXTURE11),
    REG_DEF(GL_TEXTURE12),
    REG_DEF(GL_TEXTURE13),
    REG_DEF(GL_TEXTURE14),
    REG_DEF(GL_TEXTURE15),
    REG_DEF(GL_TEXTURE16),
    REG_DEF(GL_TEXTURE17),
    REG_DEF(GL_TEXTURE18),
    REG_DEF(GL_TEXTURE19),
    REG_DEF(GL_TEXTURE20),
    REG_DEF(GL_TEXTURE21),
    REG_DEF(GL_TEXTURE22),
    REG_DEF(GL_TEXTURE23),
    REG_DEF(GL_TEXTURE24),
    REG_DEF(GL_TEXTURE25),
    REG_DEF(GL_TEXTURE26),
    REG_DEF(GL_TEXTURE27),
    REG_DEF(GL_TEXTURE28),
    REG_DEF(GL_TEXTURE29),
    REG_DEF(GL_TEXTURE30),
    REG_DEF(GL_TEXTURE31),
    REG_DEF(GL_ACTIVE_TEXTURE),
    REG_DEF(GL_REPEAT),
    REG_DEF(GL_CLAMP_TO_EDGE),
    REG_DEF(GL_MIRRORED_REPEAT),
    REG_DEF(GL_FLOAT_VEC2),
    REG_DEF(GL_FLOAT_VEC3),
    REG_DEF(GL_FLOAT_VEC4),
    REG_DEF(GL_INT_VEC2),
    REG_DEF(GL_INT_VEC3),
    REG_DEF(GL_INT_VEC4),
    REG_DEF(GL_BOOL),
    REG_DEF(GL_BOOL_VEC2),
    REG_DEF(GL_BOOL_VEC3),
    REG_DEF(GL_BOOL_VEC4),
    REG_DEF(GL_FLOAT_MAT2),
    REG_DEF(GL_FLOAT_MAT3),
    REG_DEF(GL_FLOAT_MAT4),
    REG_DEF(GL_SAMPLER_2D),
    REG_DEF(GL_SAMPLER_CUBE),
    REG_DEF(GL_VERTEX_ATTRIB_ARRAY_ENABLED),
    REG_DEF(GL_VERTEX_ATTRIB_ARRAY_SIZE),
    REG_DEF(GL_VERTEX_ATTRIB_ARRAY_STRIDE),
    REG_DEF(GL_VERTEX_ATTRIB_ARRAY_TYPE),
    REG_DEF(GL_VERTEX_ATTRIB_ARRAY_NORMALIZED),
    REG_DEF(GL_VERTEX_ATTRIB_ARRAY_POINTER),
    REG_DEF(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING),
    REG_DEF(GL_IMPLEMENTATION_COLOR_READ_TYPE), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_IMPLEMENTATION_COLOR_READ_FORMAT), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_COMPILE_STATUS),
    REG_DEF(GL_INFO_LOG_LENGTH),
    REG_DEF(GL_SHADER_SOURCE_LENGTH),
    REG_DEF(GL_SHADER_COMPILER), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_SHADER_BINARY_FORMATS), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_NUM_SHADER_BINARY_FORMATS), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_LOW_FLOAT), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_MEDIUM_FLOAT), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_HIGH_FLOAT), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_LOW_INT), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_MEDIUM_INT), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_HIGH_INT), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_FRAMEBUFFER),
    REG_DEF(GL_RENDERBUFFER),
    REG_DEF(GL_RGBA4),
    REG_DEF(GL_RGB5_A1),
    REG_DEF(GL_RGB565), // require GL_ARB_ES2_compatibility or 4.1 for desktop
    REG_DEF(GL_DEPTH_COMPONENT16),
    REG_DEF(GL_STENCIL_INDEX8),
    REG_DEF(GL_RENDERBUFFER_WIDTH),
    REG_DEF(GL_RENDERBUFFER_HEIGHT),
    REG_DEF(GL_RENDERBUFFER_INTERNAL_FORMAT),
    REG_DEF(GL_RENDERBUFFER_RED_SIZE),
    REG_DEF(GL_RENDERBUFFER_GREEN_SIZE),
    REG_DEF(GL_RENDERBUFFER_BLUE_SIZE),
    REG_DEF(GL_RENDERBUFFER_ALPHA_SIZE),
    REG_DEF(GL_RENDERBUFFER_DEPTH_SIZE),
    REG_DEF(GL_RENDERBUFFER_STENCIL_SIZE),
    REG_DEF(GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE),
    REG_DEF(GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME),
    REG_DEF(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL),
    REG_DEF(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE),
    REG_DEF(GL_COLOR_ATTACHMENT0),
    REG_DEF(GL_DEPTH_ATTACHMENT),
    REG_DEF(GL_STENCIL_ATTACHMENT),
    REG_DEF(GL_NONE),
    REG_DEF(GL_FRAMEBUFFER_COMPLETE),
    REG_DEF(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT),
    REG_DEF(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT),
    { "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS", 0x8CD9 }, // only gles2 require all attached images have the same width and height
    REG_DEF(GL_FRAMEBUFFER_UNSUPPORTED),
    REG_DEF(GL_FRAMEBUFFER_BINDING),
    REG_DEF(GL_RENDERBUFFER_BINDING),
    REG_DEF(GL_MAX_RENDERBUFFER_SIZE),
    REG_DEF(GL_INVALID_FRAMEBUFFER_OPERATION),

    // ext
    { "GL_DEPTH_COMPONENT24_OES", 0x81A6 }, // ext GL_OES_depth24, for gles2 24-bit depth components render buffer storage
    REG_DEF(GL_VERTEX_ATTRIB_ARRAY_DIVISOR_EXT), // ext GL_EXT_instanced_arrays
};

static const DefineReg gles3_defines[] = {
    REG_DEF(GL_DEPTH_COMPONENT24),
    REG_DEF(GL_VERTEX_ATTRIB_ARRAY_DIVISOR),
    REG_DEF(GL_PIXEL_PACK_BUFFER),
    REG_DEF(GL_PIXEL_UNPACK_BUFFER),
    REG_DEF(GL_PIXEL_PACK_BUFFER_BINDING),
    REG_DEF(GL_PIXEL_UNPACK_BUFFER_BINDING),
    REG_DEF(GL_STREAM_READ),
    REG_DEF(GL_STATIC_READ),
    REG_DEF(GL_DYNAMIC_READ),
    REG_DEF(GL_MAP_READ_BIT),
    REG_DEF(GL_MAP_WRITE_BIT),
    REG_DEF(GL_MAP_INVALIDATE_RANGE_BIT),
    REG_DEF(GL_MAP_INVALIDATE_BUFFER_BIT),
    REG_DEF(GL_MAP_FLUSH_EXPLICIT_BIT),
    REG_DEF(GL_MAP_UNSYNCHRONIZED_BIT),
    REG_DEF(GL_R8),
    REG_DEF(GL_RED),
};

void LFX_LuaBindGL(lua_State* L)
{
    lua_pushglobaltable(L);
    luaL_setfuncs(L, gles2_funcs, 0);
    luaL_setfuncs(L, gles3_funcs, 0);

    luaL_setintfields(L, gles2_defines, sizeof(gles2_defines) / sizeof(gles2_defines[0]));
    luaL_setintfields(L, gles3_defines, sizeof(gles3_defines) / sizeof(gles3_defines[0]));
    
    lua_pop(L, 1);
}
