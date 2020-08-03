#include "Context.h"
#include "gl_include.h"
#include "Effect.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include "stb_image.h"

#ifdef LFX_WINDOWS
#include <Windows.h>
#endif

#if defined(LFX_ANDROID) || defined(LFX_MAC) || defined(LFX_IOS) || defined(LFX_WASM)
#include <sys/stat.h>
#endif

#if defined(LFX_WASM)
#include <emscripten.h>
#endif

#define CHECK_EFFECT(effect) \
    LFX_Effect* eff = (LFX_Effect*) LFX_ObjectPool_GetObject(&thiz->effect_pool, effect); \
    if (eff == NULL) { RETURN_ERR(LFX_INVALID_EFFECT); }

LFX_RESULT LFX_Context_Init(LFX_Context* thiz)
{
    memset(thiz, 0, sizeof(LFX_Context));
    LFX_ObjectPool_Init(&thiz->effect_pool, sizeof(LFX_Effect));

#if defined(LFX_WINDOWS)
    thiz->build_platform = LFX_BUILD_PLATFORM_WINDOWS;
#elif defined(LFX_ANDROID)
    thiz->build_platform = LFX_BUILD_PLATFORM_ANDROID;
#elif defined(LFX_MAC)
    thiz->build_platform = LFX_BUILD_PLATFORM_MAC;
#elif defined(LFX_IOS)
    thiz->build_platform = LFX_BUILD_PLATFORM_IOS;
#elif defined(LFX_WASM)
    thiz->build_platform = LFX_BUILD_PLATFORM_WASM;
#else
    not implement.
#endif

    const char* version = (const char*) glGetString(GL_VERSION);
    const char* vendor = (const char*) glGetString(GL_VENDOR);
    const char* renderer = (const char*) glGetString(GL_RENDERER);

    if (version == NULL)
    {
        RETURN_ERR(LFX_INVLAID_GL_CONTEXT);
    }

    LFX_LOGI("Context Init Version: %s", LFX_VERSION_NAME);
    LFX_LOGI("GL_VERSION: %s", version);
    LFX_LOGI("GL_VENDOR: %s", vendor);
    LFX_LOGI("GL_RENDERER: %s", renderer);

    const char* es_prefix = "OpenGL ES ";
    if (strstr(version, es_prefix) == version)
    {
        // OpenGL ES x.x
        size_t es_prefix_len = strlen(es_prefix);
        thiz->gl_es = 1;
        thiz->gl_version[0] = version[es_prefix_len] - '0';
        thiz->gl_version[1] = version[es_prefix_len + 2] - '0';

        if (thiz->gl_version[0] < 2)
        {
            RETURN_ERR(LFX_NOT_SUPPORT_GL_VERSION);
        }
    }
    else if (version[1] == '.')
    {
        // x.x
        thiz->gl_es = 0;
        thiz->gl_version[0] = version[0] - '0';
        thiz->gl_version[1] = version[2] - '0';

        if (thiz->gl_version[0] < 3)
        {
            RETURN_ERR(LFX_NOT_SUPPORT_GL_VERSION);
        }
    }
    else
    {
        RETURN_ERR(LFX_UNKNOWN_GL_VERSION);
    }

    const char* glsl_version = (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);
    LFX_LOGI("GL_SHADING_LANGUAGE_VERSION: %s", glsl_version);

    if (thiz->gl_es == 0 && thiz->gl_version[0] >= 3)
    {
#if defined(GL_VERSION_3_0)
        int ext_count = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &ext_count);

        thiz->gl_extensions = malloc(sizeof(char*) * (ext_count + 1));

        for (int i = 0; i < ext_count; ++i)
        {
            const char* ext = (const char*) glGetStringi(GL_EXTENSIONS, i);
            size_t len = strlen(ext);
            char* ext_copy = malloc(len + 1);
            strcpy(ext_copy, ext);

            thiz->gl_extensions[i] = ext_copy;
        }

        thiz->gl_extensions[ext_count] = NULL;
        thiz->gl_num_extensions = ext_count;
#endif
    }
    else
    {
        const char* exts = (const char*) glGetString(GL_EXTENSIONS);
        size_t exts_len = strlen(exts);

        int ext_count = 0;
        thiz->gl_extensions = malloc(sizeof(char*) * (ext_count + 1));

        const char* begin = exts;
        const char* end = NULL;
        while (1)
        {
            end = strchr(begin, ' ');
            if (end)
            {
                size_t len = end - begin;
                if (len > 0)
                {
                    char* ext = malloc(len + 1);
                    memcpy(ext, begin, len);
                    ext[len] = 0;

                    ext_count += 1;
                    thiz->gl_extensions = realloc(thiz->gl_extensions, sizeof(char*) * (ext_count + 1));
                    thiz->gl_extensions[ext_count - 1] = ext;
                }
                begin = end + 1;
            }
            else
            {
                break;
            }
        }

        size_t left_len = exts_len - (begin - exts);
        if (left_len > 0)
        {
            char* ext = malloc(left_len + 1);
            memcpy(ext, begin, left_len);
            ext[left_len] = 0;

            ext_count += 1;
            thiz->gl_extensions = realloc(thiz->gl_extensions, sizeof(char*) * (ext_count + 1));
            thiz->gl_extensions[ext_count - 1] = ext;
        }

        thiz->gl_extensions[ext_count] = NULL;
        thiz->gl_num_extensions = ext_count;
    }

    LFX_LOGI("GL_NUM_EXTENSIONS: %d", thiz->gl_num_extensions);
    if (thiz->gl_es)
    {
        for (int i = 0; i < thiz->gl_num_extensions; ++i)
        {
            LFX_LOGI("%s", thiz->gl_extensions[i]);
        }
    }
    LFX_LOGI("Checked GL Version: %d.%d %s", thiz->gl_version[0], thiz->gl_version[1], thiz->gl_es ? "es" : "");

#ifdef LFX_ANDROID
    if (thiz->gl_es && thiz->gl_version[0] >= 3)
    {
        if (gl3stubInit())
        {
            // gl3stubInit only load 3.0 funcs
            thiz->gl_version[1] = 0;
            LFX_LOGI("Android gles 3.0 funcs load success");
        }
        else
        {
            // fallback to 2.0
            thiz->gl_version[0] = 2;
            thiz->gl_version[1] = 0;
            LFX_LOGI("Android gles 3.0 funcs load fail, fallback to 2.0");
        }
    }
#endif

    LFX_Context_CreateQuadVBO(thiz);
    LFX_Context_CreateCopyProgram(thiz);

    glFrontFace(GL_CW);
    
    if (thiz->gl_version[0] >= 3)
    {
        glGenVertexArrays(1, (GLuint*) &thiz->default_vao);
        glBindVertexArray(thiz->default_vao);
    }

    return LFX_SUCCESS;
}

void LFX_Context_Done(LFX_Context* thiz)
{
    if (thiz->quad_vbo)
    {
        glDeleteBuffers(1, (const GLuint*) &thiz->quad_vbo);
        thiz->quad_vbo = 0;
    }
    if (thiz->copy_program)
    {
        glDeleteProgram((GLuint) thiz->copy_program);
        thiz->copy_program = 0;
    }
    LFX_ObjectPool_Done(&thiz->effect_pool);
    if (thiz->gl_extensions)
    {
        int index = 0;
        while (thiz->gl_extensions[index])
        {
            free(thiz->gl_extensions[index]);
            thiz->gl_extensions[index] = NULL;
            ++index;
        }
        free(thiz->gl_extensions);
        thiz->gl_extensions = NULL;
    }
    if (thiz->default_vao)
    {
        glDeleteVertexArrays(1, (GLuint*) &thiz->default_vao);
    }

    LFX_LOGI("Context Done");
}

LFX_RESULT LFX_Context_LoadTexture2D(LFX_Context* thiz, const char* path, LFX_Texture* texture)
{
    if (path == NULL || texture == NULL)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_Path path_normalized;
    strcpy(path_normalized, path);
    LFX_Context_NormalizePath(thiz, path_normalized);

    int w = 0;
    int h = 0;
    int c = 0;
    stbi_uc* data = stbi_load(path_normalized, &w, &h, &c, 4);
    if (data == NULL)
    {
        RETURN_ERR(LFX_IMAGE_LOAD_FAIL);
    }

    texture->id = 0;
    texture->target = GL_TEXTURE_2D;
    texture->format = GL_RGBA;
    texture->width = w;
    texture->height = h;
    if (texture->filter_mode == 0)
    {
        texture->filter_mode = GL_LINEAR;
    }
    if (texture->wrap_mode == 0)
    {
        texture->wrap_mode = GL_CLAMP_TO_EDGE;
    }

    LFX_Context_CreateTexture(thiz, texture, data);

    stbi_image_free(data);

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Context_CreateTexture(LFX_Context* thiz, LFX_Texture* texture, const void* data)
{
    if (texture == NULL || texture->id != 0)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(texture->target, tex);
    glTexImage2D(texture->target, 0, texture->format, texture->width, texture->height, 0, texture->format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, texture->filter_mode);
    glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, texture->filter_mode);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, texture->wrap_mode);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, texture->wrap_mode);
    glBindTexture(texture->target, 0);

    texture->id = (int) tex;

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Context_DestroyTexture(LFX_Context* thiz, const LFX_Texture* texture)
{
    if (texture == NULL || texture->id == 0)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    glDeleteTextures(1, (const GLuint*) &texture->id);

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Context_RenderQuad(LFX_Context* thiz, const LFX_Texture* texture, const float* matrix)
{
    if (texture == NULL || texture->id == 0)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    GLuint p = (GLuint) thiz->copy_program;
    GLuint vbo = (GLuint) thiz->quad_vbo;

    glUseProgram(p);

    int loc_mvp = glGetUniformLocation(p, "uMatrix");
    if (matrix)
    {
        glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, matrix);
    }
    else
    {
        float identity[] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };
        glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, identity);
    }

    int loc_tex0 = glGetUniformLocation(p, "uTexture0");
    glUniform1i(loc_tex0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(texture->target, texture->id);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
    int loc_pos = glGetAttribLocation(p, "aPosition");
    glEnableVertexAttribArray(loc_pos);
    glVertexAttribPointer(loc_pos, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
    int loc_uv = glGetAttribLocation(p, "aTextureCoord");
    glEnableVertexAttribArray(loc_uv);
    glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (const void*) (sizeof(float) * 2));

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(loc_pos);
    glDisableVertexAttribArray(loc_uv);

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Context_LoadEffect(LFX_Context* thiz, const char* path, int* effect)
{
    if (path == NULL || effect == NULL)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_Path path_normalized;
    strcpy(path_normalized, path);
    LFX_Context_NormalizePath(thiz, path_normalized);

    int effect_id = LFX_ObjectPool_CreateObject(&thiz->effect_pool);
    CHECK_EFFECT(effect_id);
    LFX_Effect_Init(eff, thiz);

    LFX_RESULT ret = LFX_Effect_Load(eff, path_normalized);
    if (ret != LFX_SUCCESS)
    {
        LFX_Effect_Done(eff);
        LFX_ObjectPool_DestroyObject(&thiz->effect_pool, effect_id);
        return ret;
    }
    else
    {
        *effect = effect_id;
    }

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Context_DestroyEffect(LFX_Context* thiz, int effect)
{
    CHECK_EFFECT(effect);
    LFX_Effect_Done(eff);
    LFX_ObjectPool_DestroyObject(&thiz->effect_pool, effect);
    return LFX_SUCCESS;
}

LFX_RESULT LFX_Context_RenderEffect(LFX_Context* thiz, int effect, const LFX_Texture* input_texture, const LFX_Texture* output_texture, void* output_image)
{
    CHECK_EFFECT(effect);
    return LFX_Effect_Render(eff, input_texture, output_texture, output_image);
}

LFX_RESULT LFX_Context_SendEffectMessage(LFX_Context* thiz, int effect, LFX_MESSAGE_ID message_id, const char* message, char* reply_buffer, int reply_buffer_size)
{
    CHECK_EFFECT(effect);
    return LFX_Effect_SendMessage(eff, message_id, message, reply_buffer, reply_buffer_size);
}

LFX_RESULT LFX_Context_SetEffectMessageCallback(LFX_Context* thiz, int effect, LFX_EFFECT_MESSAGE_CALLBACK callback)
{
    CHECK_EFFECT(effect);
    return LFX_Effect_SetMessageCallback(eff, callback);
}

void LFX_Context_CreateQuadVBO(LFX_Context* thiz)
{
    const float quad[] = {
        -1, 1, 0, 0,
        -1, -1, 0, 1,
        1, -1, 1, 1,
        -1, 1, 0, 0,
        1, -1, 1, 1,
        1, 1, 1, 0,
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    thiz->quad_vbo = (int) vbo;
}

void LFX_Context_CreateCopyProgram(LFX_Context* thiz)
{
    const char* vs = "\
uniform mat4 uMatrix;\n\
attribute vec2 aPosition;\n\
attribute vec2 aTextureCoord;\n\
varying vec2 vTextureCoord;\n\
void main()\n\
{\n\
    gl_Position = uMatrix * vec4(aPosition, 0.0, 1.0);\n\
    vTextureCoord = aTextureCoord;\n\
}";
    const char* fs = "\
precision highp float;\n\
uniform sampler2D uTexture0;\n\
varying vec2 vTextureCoord;\n\
void main()\n\
{\n\
gl_FragColor = texture2D(uTexture0, vTextureCoord);\n\
}";

    LFX_Context_CreateProgram(thiz, vs, fs, &thiz->copy_program);
}

#if defined(LFX_WINDOWS) || defined(LFX_MAC)
static size_t StringRemove(char* dest, const char* source, const char* remove)
{
    size_t src_len = strlen(source);
    size_t remove_len = strlen(remove);
    size_t src_pos = 0;
    size_t dest_pos = 0;
    size_t copy_size = 0;

    while (1)
    {
        const char* find = strstr(&source[src_pos], remove);
        if (find)
        {
            copy_size = find - &source[src_pos];
            if (copy_size > 0)
            {
                memcpy(&dest[dest_pos], &source[src_pos], copy_size);
                dest_pos += copy_size;
            }
            src_pos += copy_size + remove_len;
        }
        else
        {
            break;
        }
    }

    copy_size = src_len - src_pos;
    if (copy_size > 0)
    {
        memcpy(&dest[dest_pos], &source[src_pos], copy_size);
        dest_pos += copy_size;
    }

    dest[dest_pos] = 0;

    return dest_pos;
}
#endif

LFX_RESULT LFX_Context_CreateShader(LFX_Context* thiz, const char* source, int type, int* shader)
{
    const char* source_processed = NULL;
    char* string_buffer = NULL;

#if defined(LFX_WINDOWS)
    const char* header = "#version 110\n#define highp\n#define mediump\n#define lowp\n";
    size_t buffer_size = strlen(header) + strlen(source) + 1;
    string_buffer = (char*) malloc(buffer_size);
    char* temp = (char*) malloc(buffer_size);
    StringRemove(temp, source, "precision highp float;");
    StringRemove(string_buffer, temp, "precision mediump float;");
    StringRemove(temp, string_buffer, "precision lowp float;");
    strcpy(string_buffer, header);
    strcat(string_buffer, temp);
    free(temp);
    source_processed = string_buffer;
#elif defined(LFX_MAC)
    const char* header = "#version 150\n#define highp\n#define mediump\n#define lowp\n"
        "#define attribute in\n#define texture2D texture\n#define gl_FragColor o_FragColor\n";
    const char* type_def = NULL;
    if (type == GL_VERTEX_SHADER)
    {
        type_def = "#define varying out\n";
    }
    else if (type == GL_FRAGMENT_SHADER)
    {
        type_def = "#define varying in\nout vec4 o_FragColor;\n";
    }
    size_t buffer_size = strlen(header) + strlen(type_def) + strlen(source) + 1;
    string_buffer = (char*) malloc(buffer_size);
    char* temp = (char*) malloc(buffer_size);
    StringRemove(temp, source, "precision highp float;");
    StringRemove(string_buffer, temp, "precision mediump float;");
    StringRemove(temp, string_buffer, "precision lowp float;");
    strcpy(string_buffer, header);
    strcat(string_buffer, type_def);
    strcat(string_buffer, temp);
    free(temp);
    source_processed = string_buffer;
#else
    source_processed = source;
#endif

    GLuint s = glCreateShader((GLenum) type);
    glShaderSource(s, 1, &source_processed, NULL);
    glCompileShader(s);

    if (string_buffer)
    {
        free(string_buffer);
        string_buffer = NULL;
    }

    GLint log_length = 0;
    glGetShaderiv(s, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0)
    {
        char* log = (char*) malloc(log_length);
        glGetShaderInfoLog(s, log_length, NULL, log);
        LFX_LOGW("Shader compile log:%s", log);
        free(log);
    }

    GLint status = GL_FALSE;
    glGetShaderiv(s, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glDeleteShader(s);
        RETURN_ERR(LFX_SHADER_COMPILE_ERROR);
    }

    *shader = (int) s;

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Context_CreateProgram(LFX_Context* thiz, const char* vs, const char* fs, int* program)
{
    if (vs == NULL || fs == NULL || program == NULL)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_RESULT ret = LFX_SUCCESS;

    int vertex_shader = 0;
    int fragment_shader = 0;
    GLuint p = 0;

    while (1)
    {
        ret = LFX_Context_CreateShader(thiz, vs, GL_VERTEX_SHADER, &vertex_shader);
        if (ret != LFX_SUCCESS)
        {
            break;
        }

        ret = LFX_Context_CreateShader(thiz, fs, GL_FRAGMENT_SHADER, &fragment_shader);
        if (ret != LFX_SUCCESS)
        {
            break;
        }

        p = glCreateProgram();
        glAttachShader(p, vertex_shader);
        glAttachShader(p, fragment_shader);
        glLinkProgram(p);

        GLint log_length = 0;
        glGetProgramiv(p, GL_INFO_LOG_LENGTH, &log_length);
        if (log_length > 0)
        {
            char* log = (char*) malloc(log_length);
            glGetProgramInfoLog(p, log_length, NULL, log);
            LFX_LOGW("Program link log:%s", log);
            free(log);
        }

        GLint status = GL_FALSE;
        glGetProgramiv(p, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            LFX_LOGE("LFX_PROGRAM_LINK_ERROR");
            ret = LFX_PROGRAM_LINK_ERROR;
            break;
        }

        break;
    }

    if (p)
    {
        glDetachShader(p, vertex_shader);
        glDetachShader(p, fragment_shader);
    }
    if (vertex_shader)
    {
        glDeleteShader((GLuint) vertex_shader);
    }
    if (fragment_shader)
    {
        glDeleteShader((GLuint) fragment_shader);
    }

    if (ret != LFX_SUCCESS)
    {
        glDeleteProgram(p);
    }
    else
    {
        *program = (int) p;
    }

    return ret;
}

void LFX_Context_NormalizePath(LFX_Context* thiz, char* path)
{
    size_t len = strlen(path);
    for (size_t i = 0; i < len; ++i)
    {
        if (path[i] == '\\')
        {
            path[i] = '/';
        }
    }
}

LFX_RESULT LFX_Context_GetFileSize(LFX_Context* thiz, const char* path, int* size)
{
    if (path == NULL || size == NULL)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_Path path_normalized;
    strcpy(path_normalized, path);
    LFX_Context_NormalizePath(thiz, path_normalized);

#if defined(LFX_WINDOWS)
    HANDLE file = CreateFile(path_normalized, GENERIC_READ,
        FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        RETURN_ERR(LFX_FILE_OPEN_FAIL);
    }

    LARGE_INTEGER file_size;
    BOOL success = GetFileSizeEx(file, &file_size);
    CloseHandle(file);

    if (!success)
    {
        RETURN_ERR(LFX_FILE_OPEN_FAIL);
    }
    else
    {
        *size = (int) file_size.QuadPart;
    }
#elif defined(LFX_ANDROID) || defined(LFX_MAC) || defined(LFX_IOS) || defined(LFX_WASM)
    struct stat st;
    int ret = stat(path_normalized, &st);
    if (ret != 0)
    {
        RETURN_ERR(LFX_FILE_OPEN_FAIL);
    }
    else
    {
        *size = (int) st.st_size;
    }
#else
    not implement.
#endif

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Context_LoadFile(LFX_Context* thiz, const char* path, void** data, int* size)
{
    if (path == NULL || data == NULL || size == NULL)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_Path path_normalized;
    strcpy(path_normalized, path);
    LFX_Context_NormalizePath(thiz, path_normalized);

    int file_size = 0;
    LFX_RESULT ret = LFX_Context_GetFileSize(thiz, path_normalized, &file_size);
    if (ret != LFX_SUCCESS)
    {
        return LFX_FILE_OPEN_FAIL;
    }

    FILE* file = fopen(path_normalized, "rb");
    if (file == NULL)
    {
        RETURN_ERR(LFX_FILE_OPEN_FAIL);
    }

    if (file_size > 0)
    {
        void* buffer = malloc(file_size);
        size_t read = fread(buffer, 1, file_size, file);
        if (read != file_size)
        {
            free(buffer);
            fclose(file);
            RETURN_ERR(LFX_FILE_READ_ERROR);
        }
        else
        {
            *data = buffer;
            *size = file_size;
        }
    }
    else
    {
        *data = NULL;
        *size = 0;
    }

    fclose(file);

    return LFX_SUCCESS;
}

#if defined(LFX_WASM)
typedef struct LoadFileContext
{
    LFX_Context* context;
    LFX_LOAD_FILE_CALLBACK callback;
    void* user_data;
} LoadFileContext;

void EMSCRIPTEN_KEEPALIVE OnLoadFileAsyncComplete(void* user_data, const char* url, void* data, int size)
{
    LoadFileContext* load = user_data;
    if (size > 0)
    {
        load->callback(LFX_SUCCESS, load->context, url, data, size, load->user_data);
    }
    else
    {
        load->callback(LFX_FAIL, load->context, url, NULL, 0, load->user_data);
    }
    free(load);
}

EM_JS(void, LoadFileAsyncJS, (void* user_data, const char* url), {
    LoadFileAsync(user_data, UTF8ToString(url));
});
#endif

LFX_RESULT LFX_Context_LoadFileAsync(LFX_Context* thiz, const char* path, LFX_LOAD_FILE_CALLBACK callback, void* user_data)
{
    if (path == NULL || callback == NULL)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

#if defined(LFX_WASM)
    LoadFileContext* load = malloc(sizeof(LoadFileContext));
    load->context = thiz;
    load->callback = callback;
    load->user_data = user_data;

    LoadFileAsyncJS(load, path);
#else
    void* data = NULL;
    int size = 0;
    LFX_RESULT ret = LFX_Context_LoadFile(thiz, path, &data, &size);
    callback(ret, thiz, path, data, size, user_data);
#endif

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Context_CheckGLExtension(LFX_Context* thiz, const char* name)
{
    if (name == NULL)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    for (int i = 0; i < thiz->gl_num_extensions; ++i)
    {
        if (strcmp(thiz->gl_extensions[i], name) == 0)
        {
            return LFX_SUCCESS;
        }
    }
    return LFX_FAIL;
}

LFX_RESULT LFX_Context_GetGLVersion(LFX_Context* thiz, int* major, int* minor, int* is_es)
{
    if (major == NULL || minor == NULL || is_es == NULL)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    *major = thiz->gl_version[0];
    *minor = thiz->gl_version[1];
    *is_es = thiz->gl_es;

    return LFX_SUCCESS;
}

LFX_RESULT LFX_Context_GetBuildPlatform(LFX_Context* thiz, LFX_BUILD_PLATFORM* platform)
{
    if (platform == NULL)
    {
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    *platform = thiz->build_platform;

    return LFX_SUCCESS;
}
