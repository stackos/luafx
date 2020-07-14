#include "luafx.h"
#include "Context.h"
#include <jni.h>
#include <android/asset_manager_jni.h>
#include <sys/stat.h>

#define JNICALL_BEGIN g_ctx.env = env;
#define JNICALL_END g_ctx.env = NULL;
#define SET_INT_FIELD(obj, cls, name, i) (*env)->SetIntField(env, obj, (*env)->GetFieldID(env, cls, name, "I"), i)
#define GET_INT_FIELD(obj, cls, name) (*env)->GetIntField(env, obj, (*env)->GetFieldID(env, cls, name, "I"))
#define MESSAGE_BUFFER_SIZE 8192

typedef struct JNIContext
{
    JavaVM* vm;
    jclass luafx_clz;
    JNIEnv* env;
} JNIContext;
static JNIContext g_ctx;
static char g_message_buffer[MESSAGE_BUFFER_SIZE];

static void GetTextureFromJava(JNIEnv* env, jobject jtexture, LFX_Texture* texture);
static LFX_RESULT JNIEffectMessageCallback(LFX_MESSAGE_ID message_id, const char* message, char* reply_buffer, int reply_buffer_size);
static void JNILogCallback(const char* message);

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        return JNI_ERR;
    }

    memset(&g_ctx, 0, sizeof(g_ctx));
    g_ctx.vm = vm;
    jclass clz = (*env)->FindClass(env, "com/luafx/lib/LuaFX");
    g_ctx.luafx_clz = (*env)->NewGlobalRef(env, clz);

    LFX_SetLogCallback(JNILogCallback);
    LFX_LOGI("JNI_OnLoad");

    return  JNI_VERSION_1_6;
}

JNIEXPORT jint JNICALL Java_com_luafx_lib_LuaFX_extractAssetsDir(JNIEnv* env, jclass luafx_cls,
        jobject jassetManager,
        jstring jsrcDir,
        jstring jdstDir)
{
    JNICALL_BEGIN
    if (jassetManager == NULL || jsrcDir == NULL || jdstDir == NULL)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_Path src_dir;
    LFX_Path dst_dir;

    const char* str = (*env)->GetStringUTFChars(env, jsrcDir, NULL);
    strcpy(src_dir, str);
    (*env)->ReleaseStringUTFChars(env, jsrcDir, str);

    str = (*env)->GetStringUTFChars(env, jdstDir, NULL);
    strcpy(dst_dir, str);
    (*env)->ReleaseStringUTFChars(env, jdstDir, str);

    LFX_Vector files;
    LFX_Vector_Init(&files, sizeof(LFX_Path));

    AAssetManager* mgr = AAssetManager_fromJava(env, jassetManager);

    AAssetDir* dir = AAssetManager_openDir(mgr, src_dir);
    const char* file_name = NULL;
    while ((file_name = AAssetDir_getNextFileName(dir)) != NULL)
    {
        LFX_Path name;
        strcpy(name, file_name);
        LFX_Vector_AddElement(&files, &name);
    }
    AAssetDir_close(dir);

    int file_count = LFX_Vector_GetCount(&files);
    if (file_count > 0)
    {
        mkdir(dst_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    LFX_RESULT ret = LFX_SUCCESS;
    for (int i = 0; i < file_count; ++i)
    {
        const char* file_i = (const char*) LFX_Vector_GetElement(&files, i);

        LFX_Path file;
        sprintf(file, "%s/%s", src_dir, file_i);

        AAsset* asset = AAssetManager_open(mgr, file, AASSET_MODE_BUFFER);
        if (asset)
        {
            size_t size = (size_t) AAsset_getLength(asset);
            const void* buffer = AAsset_getBuffer(asset);

            LFX_Path out_file;
            sprintf(out_file, "%s/%s", dst_dir, file_i);
            FILE* f = fopen(out_file, "wb");
            if (f)
            {
                fwrite(buffer, 1, size, f);
                fclose(f);

                LFX_LOGI("Extract asset file to: %s", out_file);
            }
            else
            {
                ret = LFX_FILE_OPEN_FAIL;
                LFX_LOGE("LFX_FILE_OPEN_FAIL");
            }

            AAsset_close(asset);
        }
        else
        {
            ret = LFX_ASSET_OPEN_FAIL;
            LFX_LOGE("LFX_ASSET_OPEN_FAIL");
        }
    }

    LFX_Vector_Done(&files);

    JNICALL_END
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_luafx_lib_LuaFX_createContext(JNIEnv* env, jclass luafx_cls,
    jintArray jcontextContainer)
{
    JNICALL_BEGIN
    if (jcontextContainer == NULL)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    int context = 0;
    LFX_RESULT ret = LFX_CreateContext(&context);
    if (ret != LFX_SUCCESS)
    {
        JNICALL_END
        return ret;
    }

    jsize len = (*env)->GetArrayLength(env, jcontextContainer);
    if (len <= 0)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    (*env)->SetIntArrayRegion(env, jcontextContainer, 0, 1, &context);

    JNICALL_END
    return LFX_SUCCESS;
}

JNIEXPORT jint JNICALL
Java_com_luafx_lib_LuaFX_destroyContext(JNIEnv* env, jclass luafx_cls,
    jint jcontext)
{
    JNICALL_BEGIN
    if (jcontext <= 0)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_RESULT ret = LFX_DestroyContext(jcontext);

    JNICALL_END
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_luafx_lib_LuaFX_loadTexture2D(JNIEnv* env, jclass luafx_cls,
    jint jcontext,
    jstring jpath,
    jobject jtexture)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jpath == NULL || jtexture == NULL)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    const char* path = (*env)->GetStringUTFChars(env, jpath, NULL);
    if (path == NULL)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_Texture texture = { 0 };
    LFX_RESULT ret = LFX_LoadTexture2D(jcontext, path, &texture);

    (*env)->ReleaseStringUTFChars(env, jpath, path);

    if (ret != LFX_SUCCESS)
    {
        JNICALL_END
        return ret;
    }

    jclass cls = (*env)->GetObjectClass(env, jtexture);
    SET_INT_FIELD(jtexture, cls, "id", texture.id);
    SET_INT_FIELD(jtexture, cls, "target", texture.target);
    SET_INT_FIELD(jtexture, cls, "format", texture.format);
    SET_INT_FIELD(jtexture, cls, "width", texture.width);
    SET_INT_FIELD(jtexture, cls, "height", texture.height);
    SET_INT_FIELD(jtexture, cls, "filterMode", texture.filter_mode);
    SET_INT_FIELD(jtexture, cls, "wrapMode", texture.wrap_mode);
    (*env)->DeleteLocalRef(env, cls);

    JNICALL_END
    return LFX_SUCCESS;
}

JNIEXPORT jint JNICALL
Java_com_luafx_lib_LuaFX_createTexture(JNIEnv* env, jclass luafx_cls,
    jint jcontext,
    jobject jtexture)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jtexture == NULL)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_Texture texture = { 0 };
    GetTextureFromJava(env, jtexture, &texture);

    LFX_RESULT ret = LFX_CreateTexture(jcontext, &texture);
    if (ret != LFX_SUCCESS)
    {
        JNICALL_END
        return ret;
    }

    jclass cls = (*env)->GetObjectClass(env, jtexture);
    SET_INT_FIELD(jtexture, cls, "id", texture.id);
    (*env)->DeleteLocalRef(env, cls);

    JNICALL_END
    return LFX_SUCCESS;
}

JNIEXPORT jint JNICALL
Java_com_luafx_lib_LuaFX_destroyTexture(JNIEnv* env, jclass luafx_cls,
    jint jcontext,
    jobject jtexture)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jtexture == NULL)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_Texture texture = { 0 };
    GetTextureFromJava(env, jtexture, &texture);

    JNICALL_END
    return LFX_DestroyTexture(jcontext, &texture);
}

JNIEXPORT jint JNICALL
Java_com_luafx_lib_LuaFX_renderQuad(JNIEnv* env, jclass luafx_cls,
    jint jcontext,
    jobject jtexture,
    jfloatArray jmatrix)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jtexture == NULL || jmatrix == NULL)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_Texture texture = { 0 };
    GetTextureFromJava(env, jtexture, &texture);

    jsize len = (*env)->GetArrayLength(env, jmatrix);
    if (len < 16)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    jfloat* matrix = (*env)->GetFloatArrayElements(env, jmatrix, NULL);
    if (matrix == NULL)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_RESULT ret = LFX_RenderQuad(jcontext, &texture, matrix);

    (*env)->ReleaseFloatArrayElements(env, jmatrix, matrix, JNI_ABORT);

    JNICALL_END
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_luafx_lib_LuaFX_loadEffect(JNIEnv* env, jclass luafx_cls,
    jint jcontext,
    jstring jpath,
    jintArray jeffectContainer)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jpath == NULL || jeffectContainer == NULL)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    const char* path = (*env)->GetStringUTFChars(env, jpath, NULL);
    if (path == NULL)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    int effect = 0;
    LFX_RESULT ret = LFX_LoadEffect(jcontext, path, &effect);

    (*env)->ReleaseStringUTFChars(env, jpath, path);

    if (ret != LFX_SUCCESS)
    {
        JNICALL_END
        return ret;
    }

    (*env)->SetIntArrayRegion(env, jeffectContainer, 0, 1, &effect);

    LFX_SetEffectMessageCallback(jcontext, effect, JNIEffectMessageCallback);

    JNICALL_END
    return LFX_SUCCESS;
}

JNIEXPORT jint JNICALL
Java_com_luafx_lib_LuaFX_destroyEffect(JNIEnv* env, jclass luafx_cls,
    jint jcontext,
    jint jeffect)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jeffect <= 0)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_RESULT ret = LFX_DestroyEffect(jcontext, jeffect);

    JNICALL_END
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_luafx_lib_LuaFX_renderEffect(JNIEnv* env, jclass luafx_cls,
    jint jcontext,
    jint jeffect,
    jobject jinputTexture,
    jobject joutputTexture,
    jbyteArray joutputImage)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jeffect <= 0 || jinputTexture == NULL || joutputTexture == NULL)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_Texture input_texture = { 0 };
    GetTextureFromJava(env, jinputTexture, &input_texture);

    LFX_Texture output_texture = { 0 };
    GetTextureFromJava(env, joutputTexture, &output_texture);

    void* output_image = NULL;
    if (joutputImage)
    {
        output_image = (*env)->GetByteArrayElements(env, joutputImage, NULL);
    }

    LFX_RESULT ret = LFX_RenderEffect(jcontext, jeffect, &input_texture, &output_texture, output_image);

    if (output_image)
    {
        (*env)->ReleaseByteArrayElements(env, joutputImage, output_image, 0);
    }

    JNICALL_END
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_luafx_lib_LuaFX_sendEffectMessage(JNIEnv* env, jclass luafx_cls,
    jint jcontext,
    jint jeffect,
    jint jmessageId,
    jstring jmessage,
    jobjectArray jreplyContainer)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jeffect <= 0 || jmessage == NULL || jreplyContainer == NULL)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    jsize len = (*env)->GetArrayLength(env, jreplyContainer);
    if (len <= 0)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    const char* message = (*env)->GetStringUTFChars(env, jmessage, NULL);
    if (message == NULL)
    {
        JNICALL_END
        RETURN_ERR(LFX_INVALID_INPUT);
    }

    LFX_RESULT ret = LFX_SendEffectMessage(jcontext, jeffect, jmessageId, message, g_message_buffer, MESSAGE_BUFFER_SIZE);

    (*env)->ReleaseStringUTFChars(env, jmessage, message);

    if (ret != LFX_SUCCESS)
    {
        JNICALL_END
        return ret;
    }

    jstring jreply = (*env)->NewStringUTF(env, g_message_buffer);
    (*env)->SetObjectArrayElement(env, jreplyContainer, 0, jreply);

    JNICALL_END
    return ret;
}

static void GetTextureFromJava(JNIEnv* env, jobject jtexture, LFX_Texture* texture)
{
    jclass cls = (*env)->GetObjectClass(env, jtexture);
    texture->id = GET_INT_FIELD(jtexture, cls, "id");
    texture->target = GET_INT_FIELD(jtexture, cls, "target");
    texture->format = GET_INT_FIELD(jtexture, cls, "format");
    texture->width = GET_INT_FIELD(jtexture, cls, "width");
    texture->height = GET_INT_FIELD(jtexture, cls, "height");
    texture->filter_mode = GET_INT_FIELD(jtexture, cls, "filterMode");
    texture->wrap_mode = GET_INT_FIELD(jtexture, cls, "wrapMode");
    (*env)->DeleteLocalRef(env, cls);
}

static LFX_RESULT JNIEffectMessageCallback(LFX_MESSAGE_ID message_id, const char* message, char* reply_buffer, int reply_buffer_size)
{
    JNIEnv* env = g_ctx.env;
    if (env == NULL)
    {
        if ((*g_ctx.vm)->GetEnv(g_ctx.vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
        {
            RETURN_ERR(LFX_JNI_ENV_ERROR);
        }
    }

    jmethodID mid = (*env)->GetStaticMethodID(env, g_ctx.luafx_clz, "effectMessageCallback", "(ILjava/lang/String;)Ljava/lang/String;");
    jstring jmessage = (*env)->NewStringUTF(env, message);
    jstring jreply = (*env)->CallStaticObjectMethod(env, g_ctx.luafx_clz, mid, message_id, jmessage);
    (*env)->DeleteLocalRef(env, jmessage);

    LFX_RESULT ret = LFX_SUCCESS;
    if (reply_buffer && reply_buffer_size > 0)
    {
        if (jreply)
        {
            const char* reply = (*env)->GetStringUTFChars(env, jreply, NULL);
            if (reply)
            {
                size_t reply_len = strlen(reply);
                if (reply_len < reply_buffer_size)
                {
                    memcpy(reply_buffer, reply, reply_len);
                    reply_buffer[reply_len] = 0;
                }
                else
                {
                    LFX_LOGE("LFX_MESSAGE_REPLY_BUFFER_NOT_ENOUGH");
                    ret = LFX_MESSAGE_REPLY_BUFFER_NOT_ENOUGH;
                }

                (*env)->ReleaseStringUTFChars(env, jreply, reply);
            }
            else
            {
                LFX_LOGE("LFX_JNI_ENV_ERROR");
                ret = LFX_JNI_ENV_ERROR;
            }

            (*env)->DeleteLocalRef(env, jreply);
        }
        else
        {
            reply_buffer[0] = 0;
        }
    }

    return ret;
}

static void JNILogCallback(const char* message)
{
    JNIEnv* env = g_ctx.env;
    if (env == NULL)
    {
        if ((*g_ctx.vm)->GetEnv(g_ctx.vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
        {
            return;
        }
    }

    jmethodID mid = (*env)->GetStaticMethodID(env, g_ctx.luafx_clz, "logCallback", "(Ljava/lang/String;)V");
    jstring jmessage = (*env)->NewStringUTF(env, message);
    (*env)->CallStaticVoidMethod(env, g_ctx.luafx_clz, mid, jmessage);
    (*env)->DeleteLocalRef(env, jmessage);
}
