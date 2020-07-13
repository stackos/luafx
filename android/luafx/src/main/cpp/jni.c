#include "oflite.h"
#include "Context.h"
#include "of_pose.h"
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
    jclass ofl_clz;
    JNIEnv* env;
} JNIContext;
static JNIContext g_ctx;
static char g_message_buffer[MESSAGE_BUFFER_SIZE];

static void GetTextureFromJava(JNIEnv* env, jobject jtexture, OFL_Texture* texture);
static OFL_RESULT JNIEffectMessageCallback(OFL_MESSAGE_ID message_id, const char* message, char* reply_buffer, int reply_buffer_size);
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
    jclass clz = (*env)->FindClass(env, "com/oflite/lib/OFLite");
    g_ctx.ofl_clz = (*env)->NewGlobalRef(env, clz);

    OFL_SetLogCallback(JNILogCallback);
    OFL_LOGI("JNI_OnLoad");

    return  JNI_VERSION_1_6;
}

JNIEXPORT jint JNICALL Java_com_oflite_lib_OFLite_extractAssetsDir(JNIEnv* env, jclass ofl_cls,
        jobject jassetManager,
        jstring jsrcDir,
        jstring jdstDir)
{
    JNICALL_BEGIN
    if (jassetManager == NULL || jsrcDir == NULL || jdstDir == NULL)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    OFL_Path src_dir;
    OFL_Path dst_dir;

    const char* str = (*env)->GetStringUTFChars(env, jsrcDir, NULL);
    strcpy(src_dir, str);
    (*env)->ReleaseStringUTFChars(env, jsrcDir, str);

    str = (*env)->GetStringUTFChars(env, jdstDir, NULL);
    strcpy(dst_dir, str);
    (*env)->ReleaseStringUTFChars(env, jdstDir, str);

    OFL_Vector files;
    OFL_Vector_Init(&files, sizeof(OFL_Path));

    AAssetManager* mgr = AAssetManager_fromJava(env, jassetManager);

    AAssetDir* dir = AAssetManager_openDir(mgr, src_dir);
    const char* file_name = NULL;
    while ((file_name = AAssetDir_getNextFileName(dir)) != NULL)
    {
        OFL_Path name;
        strcpy(name, file_name);
        OFL_Vector_AddElement(&files, &name);
    }
    AAssetDir_close(dir);

    int file_count = OFL_Vector_GetCount(&files);
    if (file_count > 0)
    {
        mkdir(dst_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    OFL_RESULT ret = OFL_SUCCESS;
    for (int i = 0; i < file_count; ++i)
    {
        const char* file_i = (const char*) OFL_Vector_GetElement(&files, i);

        OFL_Path file;
        sprintf(file, "%s/%s", src_dir, file_i);

        AAsset* asset = AAssetManager_open(mgr, file, AASSET_MODE_BUFFER);
        if (asset)
        {
            size_t size = (size_t) AAsset_getLength(asset);
            const void* buffer = AAsset_getBuffer(asset);

            OFL_Path out_file;
            sprintf(out_file, "%s/%s", dst_dir, file_i);
            FILE* f = fopen(out_file, "wb");
            if (f)
            {
                fwrite(buffer, 1, size, f);
                fclose(f);

                OFL_LOGI("Extract asset file to: %s", out_file);
            }
            else
            {
                ret = OFL_FILE_OPEN_FAIL;
                OFL_LOGE("OFL_FILE_OPEN_FAIL");
            }

            AAsset_close(asset);
        }
        else
        {
            ret = OFL_ASSET_OPEN_FAIL;
            OFL_LOGE("OFL_ASSET_OPEN_FAIL");
        }
    }

    OFL_Vector_Done(&files);

    JNICALL_END
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_oflite_lib_OFLite_createContext(JNIEnv* env, jclass ofl_cls,
    jintArray jcontextContainer)
{
    JNICALL_BEGIN
    if (jcontextContainer == NULL)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    int context = 0;
    OFL_RESULT ret = OFL_CreateContext(&context);
    if (ret != OFL_SUCCESS)
    {
        JNICALL_END
        return ret;
    }

    jsize len = (*env)->GetArrayLength(env, jcontextContainer);
    if (len <= 0)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    (*env)->SetIntArrayRegion(env, jcontextContainer, 0, 1, &context);

    JNICALL_END
    return OFL_SUCCESS;
}

JNIEXPORT jint JNICALL
Java_com_oflite_lib_OFLite_destroyContext(JNIEnv* env, jclass ofl_cls,
    jint jcontext)
{
    JNICALL_BEGIN
    if (jcontext <= 0)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    OFL_RESULT ret = OFL_DestroyContext(jcontext);

    JNICALL_END
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_oflite_lib_OFLite_loadTexture2D(JNIEnv* env, jclass ofl_cls,
    jint jcontext,
    jstring jpath,
    jobject jtexture)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jpath == NULL || jtexture == NULL)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    const char* path = (*env)->GetStringUTFChars(env, jpath, NULL);
    if (path == NULL)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    OFL_Texture texture = { 0 };
    OFL_RESULT ret = OFL_LoadTexture2D(jcontext, path, &texture);

    (*env)->ReleaseStringUTFChars(env, jpath, path);

    if (ret != OFL_SUCCESS)
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
    return OFL_SUCCESS;
}

JNIEXPORT jint JNICALL
Java_com_oflite_lib_OFLite_createTexture(JNIEnv* env, jclass ofl_cls,
    jint jcontext,
    jobject jtexture)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jtexture == NULL)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    OFL_Texture texture = { 0 };
    GetTextureFromJava(env, jtexture, &texture);

    OFL_RESULT ret = OFL_CreateTexture(jcontext, &texture);
    if (ret != OFL_SUCCESS)
    {
        JNICALL_END
        return ret;
    }

    jclass cls = (*env)->GetObjectClass(env, jtexture);
    SET_INT_FIELD(jtexture, cls, "id", texture.id);
    (*env)->DeleteLocalRef(env, cls);

    JNICALL_END
    return OFL_SUCCESS;
}

JNIEXPORT jint JNICALL
Java_com_oflite_lib_OFLite_destroyTexture(JNIEnv* env, jclass ofl_cls,
    jint jcontext,
    jobject jtexture)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jtexture == NULL)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    OFL_Texture texture = { 0 };
    GetTextureFromJava(env, jtexture, &texture);

    JNICALL_END
    return OFL_DestroyTexture(jcontext, &texture);
}

JNIEXPORT jint JNICALL
Java_com_oflite_lib_OFLite_renderQuad(JNIEnv* env, jclass ofl_cls,
    jint jcontext,
    jobject jtexture,
    jfloatArray jmatrix)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jtexture == NULL || jmatrix == NULL)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    OFL_Texture texture = { 0 };
    GetTextureFromJava(env, jtexture, &texture);

    jsize len = (*env)->GetArrayLength(env, jmatrix);
    if (len < 16)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    jfloat* matrix = (*env)->GetFloatArrayElements(env, jmatrix, NULL);
    if (matrix == NULL)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    OFL_RESULT ret = OFL_RenderQuad(jcontext, &texture, matrix);

    (*env)->ReleaseFloatArrayElements(env, jmatrix, matrix, JNI_ABORT);

    JNICALL_END
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_oflite_lib_OFLite_loadEffect(JNIEnv* env, jclass ofl_cls,
    jint jcontext,
    jstring jpath,
    jintArray jeffectContainer)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jpath == NULL || jeffectContainer == NULL)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    const char* path = (*env)->GetStringUTFChars(env, jpath, NULL);
    if (path == NULL)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    int effect = 0;
    OFL_RESULT ret = OFL_LoadEffect(jcontext, path, &effect);

    (*env)->ReleaseStringUTFChars(env, jpath, path);

    if (ret != OFL_SUCCESS)
    {
        JNICALL_END
        return ret;
    }

    (*env)->SetIntArrayRegion(env, jeffectContainer, 0, 1, &effect);

    OFL_SetEffectMessageCallback(jcontext, effect, JNIEffectMessageCallback);

    JNICALL_END
    return OFL_SUCCESS;
}

JNIEXPORT jint JNICALL
Java_com_oflite_lib_OFLite_destroyEffect(JNIEnv* env, jclass ofl_cls,
    jint jcontext,
    jint jeffect)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jeffect <= 0)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    OFL_RESULT ret = OFL_DestroyEffect(jcontext, jeffect);

    JNICALL_END
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_oflite_lib_OFLite_renderEffect(JNIEnv* env, jclass ofl_cls,
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
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    OFL_Texture input_texture = { 0 };
    GetTextureFromJava(env, jinputTexture, &input_texture);

    OFL_Texture output_texture = { 0 };
    GetTextureFromJava(env, joutputTexture, &output_texture);

    void* output_image = NULL;
    if (joutputImage)
    {
        output_image = (*env)->GetByteArrayElements(env, joutputImage, NULL);
    }

    OFL_RESULT ret = OFL_RenderEffect(jcontext, jeffect, &input_texture, &output_texture, output_image);

    if (output_image)
    {
        (*env)->ReleaseByteArrayElements(env, joutputImage, output_image, 0);
    }

    JNICALL_END
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_oflite_lib_OFLite_sendEffectMessage(JNIEnv* env, jclass ofl_cls,
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
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    jsize len = (*env)->GetArrayLength(env, jreplyContainer);
    if (len <= 0)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    const char* message = (*env)->GetStringUTFChars(env, jmessage, NULL);
    if (message == NULL)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    OFL_RESULT ret = OFL_SendEffectMessage(jcontext, jeffect, jmessageId, message, g_message_buffer, MESSAGE_BUFFER_SIZE);

    (*env)->ReleaseStringUTFChars(env, jmessage, message);

    if (ret != OFL_SUCCESS)
    {
        JNICALL_END
        return ret;
    }

    jstring jreply = (*env)->NewStringUTF(env, g_message_buffer);
    (*env)->SetObjectArrayElement(env, jreplyContainer, 0, jreply);

    JNICALL_END
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_oflite_lib_OFLite_headPoseEstimate(JNIEnv* env, jclass ofl_cls,
    jint jcontext,
    jobject jfaceData)
{
    JNICALL_BEGIN
    if (jcontext <= 0 || jfaceData == NULL)
    {
        JNICALL_END
        RETURN_ERR(OFL_INVALID_INPUT);
    }

    OF_FrameData data = { 0 };

    jclass jfaceData_cls = (*env)->GetObjectClass(env, jfaceData);
    data.faceFrameDataArr.faceCount = (OFUInt32) GET_INT_FIELD(jfaceData, jfaceData_cls, "faceCount");
    jfieldID face_array_id = (*env)->GetFieldID(env, jfaceData_cls, "faceArray", "[Lcom/oflite/lib/OFLite$OFL_FaceItem;");
    jobjectArray jface_array = (jobjectArray) (*env)->GetObjectField(env, jfaceData, face_array_id);
    int jface_array_len = (*env)->GetArrayLength(env, jface_array);
    if (data.faceFrameDataArr.faceCount > jface_array_len)
    {
        data.faceFrameDataArr.faceCount = (OFUInt32) jface_array_len;
    }
    if (data.faceFrameDataArr.faceCount > OF_MAX_FACEFRAMEDATA_SIZE)
    {
        data.faceFrameDataArr.faceCount = OF_MAX_FACEFRAMEDATA_SIZE;
    }

    for (int i = 0; i < data.faceFrameDataArr.faceCount; ++i)
    {
        OF_FaceFrameData* face_data = &data.faceFrameDataArr.faceItemArr[i];
        jobject jface = (*env)->GetObjectArrayElement(env, jface_array, i);
        jclass jface_cls = (*env)->GetObjectClass(env, jface);
        face_data->facePointsCount = (OFUInt32) GET_INT_FIELD(jface, jface_cls, "pointCount");

        jfieldID point_array_id = (*env)->GetFieldID(env, jface_cls, "pointArray", "[F");
        jfloatArray jpoint_array = (*env)->GetObjectField(env, jface, point_array_id);
        int jpoint_array_len = (*env)->GetArrayLength(env, jpoint_array);
        if (face_data->facePointsCount > jpoint_array_len / 2)
        {
            face_data->facePointsCount = (OFUInt32) jpoint_array_len / 2;
        }
        float* point_array = (*env)->GetFloatArrayElements(env, jpoint_array, NULL);
        memcpy(face_data->facePoints, point_array, sizeof(float) * face_data->facePointsCount * 2);
        (*env)->ReleaseFloatArrayElements(env, jpoint_array, point_array, JNI_ABORT);
        (*env)->DeleteLocalRef(env, jpoint_array);

        (*env)->DeleteLocalRef(env, jface_cls);
        (*env)->DeleteLocalRef(env, jface);
    }

    OF_HeadPoseEstimate((OFHandle) jcontext, &data);

    for (int i = 0; i < data.faceFrameDataArr.faceCount; ++i)
    {
        OF_FaceFrameData* face_data = &data.faceFrameDataArr.faceItemArr[i];
        jobject jface = (*env)->GetObjectArrayElement(env, jface_array, i);
        jclass jface_cls = (*env)->GetObjectClass(env, jface);

        jfieldID head_pose_id = (*env)->GetFieldID(env, jface_cls, "headPose", "Lcom/oflite/lib/OFLite$OFL_FaceHeadPose;");
        jobject jhead_pose = (*env)->GetObjectField(env, jface, head_pose_id);
        jclass jhead_pose_cls = (*env)->GetObjectClass(env, jhead_pose);

        jfieldID model_view_matrix_id = (*env)->GetFieldID(env, jhead_pose_cls, "modelViewMatrix", "[F");
        jfloatArray jmodel_view_matrix = (*env)->GetObjectField(env, jhead_pose, model_view_matrix_id);
        float* model_view_matrix = (*env)->GetFloatArrayElements(env, jmodel_view_matrix, NULL);
        memcpy(model_view_matrix, face_data->headPose.modelViewMat, sizeof(float) * 16);
        (*env)->ReleaseFloatArrayElements(env, jmodel_view_matrix, model_view_matrix, 0);
        (*env)->DeleteLocalRef(env, jmodel_view_matrix);

        jfieldID projection_matrix_id = (*env)->GetFieldID(env, jhead_pose_cls, "projectionMatrix", "[F");
        jfloatArray jprojection_matrix = (*env)->GetObjectField(env, jhead_pose, projection_matrix_id);
        float* projection_matrix = (*env)->GetFloatArrayElements(env, jprojection_matrix, NULL);
        memcpy(projection_matrix, face_data->headPose.projectionMat, sizeof(float) * 16);
        (*env)->ReleaseFloatArrayElements(env, jprojection_matrix, projection_matrix, 0);
        (*env)->DeleteLocalRef(env, jprojection_matrix);

        (*env)->DeleteLocalRef(env, jhead_pose_cls);
        (*env)->DeleteLocalRef(env, jhead_pose);
        (*env)->DeleteLocalRef(env, jface_cls);
        (*env)->DeleteLocalRef(env, jface);
    }

    (*env)->DeleteLocalRef(env, jface_array);
    (*env)->DeleteLocalRef(env, jfaceData_cls);

    JNICALL_END
    return OFL_SUCCESS;
}

JNIEXPORT jint JNICALL
Java_com_oflite_lib_OFLite_resetHeadPoseEstimate(JNIEnv* env, jclass ofl_cls,
    jint jcontext)
{
    OF_ResetHeadPoseEstimate();
    return OFL_SUCCESS;
}

static void GetTextureFromJava(JNIEnv* env, jobject jtexture, OFL_Texture* texture)
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

static OFL_RESULT JNIEffectMessageCallback(OFL_MESSAGE_ID message_id, const char* message, char* reply_buffer, int reply_buffer_size)
{
    JNIEnv* env = g_ctx.env;
    if (env == NULL)
    {
        if ((*g_ctx.vm)->GetEnv(g_ctx.vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
        {
            RETURN_ERR(OFL_JNI_ENV_ERROR);
        }
    }

    jmethodID mid = (*env)->GetStaticMethodID(env, g_ctx.ofl_clz, "effectMessageCallback", "(ILjava/lang/String;)Ljava/lang/String;");
    jstring jmessage = (*env)->NewStringUTF(env, message);
    jstring jreply = (*env)->CallStaticObjectMethod(env, g_ctx.ofl_clz, mid, message_id, jmessage);
    (*env)->DeleteLocalRef(env, jmessage);

    OFL_RESULT ret = OFL_SUCCESS;
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
                    OFL_LOGE("OFL_MESSAGE_REPLY_BUFFER_NOT_ENOUGH");
                    ret = OFL_MESSAGE_REPLY_BUFFER_NOT_ENOUGH;
                }

                (*env)->ReleaseStringUTFChars(env, jreply, reply);
            }
            else
            {
                OFL_LOGE("OFL_JNI_ENV_ERROR");
                ret = OFL_JNI_ENV_ERROR;
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

    jmethodID mid = (*env)->GetStaticMethodID(env, g_ctx.ofl_clz, "logCallback", "(Ljava/lang/String;)V");
    jstring jmessage = (*env)->NewStringUTF(env, message);
    (*env)->CallStaticVoidMethod(env, g_ctx.ofl_clz, mid, jmessage);
    (*env)->DeleteLocalRef(env, jmessage);
}
