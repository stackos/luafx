package com.oflite.lib;

import android.content.res.AssetManager;

public class OFLite {
    public static final int OFL_INVALID_HANDLE = 0;

    // result code
    public static final int OFL_SUCCESS = 0;
    public static final int OFL_FAIL = 1;
    public static final int OFL_INVALID_CONTEXT = 2;
    public static final int OFL_INVALID_INPUT = 3;
    public static final int OFL_IMAGE_LOAD_FAIL = 4;
    public static final int OFL_SHADER_COMPILE_ERROR = 5;
    public static final int OFL_PROGRAM_LINK_ERROR = 6;
    public static final int OFL_GL_ERROR = 7;
    public static final int OFL_INVALID_EFFECT = 8;
    public static final int OFL_FILE_OPEN_FAIL = 9;
    public static final int OFL_FILE_READ_ERROR = 10;
    public static final int OFL_LUA_LOAD_ERROR = 11;
    public static final int OFL_LUA_NO_FUNCTION = 12;
    public static final int OFL_LUA_RUN_ERROR = 13;
    public static final int OFL_INVLAID_GL_CONTEXT = 14;
    public static final int OFL_UNKNOWN_GL_VERSION = 15;
    public static final int OFL_NOT_SUPPORT_GL_VERSION = 16;
    public static final int OFL_MESSAGE_REPLY_BUFFER_NOT_ENOUGH = 17;
    public static final int OFL_JNI_ENV_ERROR = 18;
    public static final int OFL_ASSET_OPEN_FAIL = 19;
    public static final int OFL_EFFECT_FILE_ERROR = 20;
    public static final int OFL_EFFECT_VERSION_ERROR = 21;

    // message id
    public static final int OFL_MESSAGE_ID_SET_FACE_DATA = -1;

    public static class OFL_Texture {
        public int id = 0;
        public int target = 0;
        public int format = 0;
        public int width = 0;
        public int height = 0;
        public int filterMode = 0;
        public int wrapMode = 0;
    }

    public static class OFL_FaceHeadPose {
        public float[] modelViewMatrix = new float[16];
        public float[] projectionMatrix = new float[16];
    }

    public static class OFL_FaceItem {
        public int pointCount = 106;
        public float[] pointArray = new float[212];
        public OFL_FaceHeadPose headPose = new OFL_FaceHeadPose();
    }

    public static class OFL_FaceData {
        public int faceCount = 0;
        public OFL_FaceItem[] faceArray = new OFL_FaceItem[5];

        public OFL_FaceData() {
            for (int i = 0; i < faceArray.length; ++i) {
                faceArray[i] = new OFL_FaceItem();
            }
        }
    }

    // message
    public interface OFL_EffectMessageCallback {
        String onRecieve(int messageId, String message);
    }

    private static OFL_EffectMessageCallback mEffectMessageCallback = null;

    private static String effectMessageCallback(int messageId, String message) {
        if (mEffectMessageCallback != null) {
            return mEffectMessageCallback.onRecieve(messageId, message);
        }
        return null;
    }

    public static int setEffectMessageCallback(OFL_EffectMessageCallback callback) {
        mEffectMessageCallback = callback;
        return OFL_SUCCESS;
    }

    // log
    public interface OFL_LogCallback {
        void log(String message);
    }

    private static OFL_LogCallback mLogCallback = null;

    private static void logCallback(String message) {
        if (mLogCallback != null) {
            mLogCallback.log(message);
        }
    }

    public static int setLogCallback(OFL_LogCallback callback) {
        mLogCallback = callback;
        return OFL_SUCCESS;
    }

    // native methods
    public static native int extractAssetsDir(AssetManager assetManager, String srcDir, String dstDir);
    public static native int createContext(int[] contextContainer);
    public static native int destroyContext(int context);
    public static native int loadTexture2D(int context, String path, OFL_Texture texture);
    public static native int createTexture(int context, OFL_Texture texture);
    public static native int destroyTexture(int context, OFL_Texture texture);
    public static native int renderQuad(int context, OFL_Texture texture, float[] matrix);
    public static native int loadEffect(int context, String path, int[] effectContainer);
    public static native int destroyEffect(int context, int effect);
    public static native int renderEffect(int context, int effect, OFL_Texture inputTexture, OFL_Texture outputTexture, byte[] outputImage);
    public static native int sendEffectMessage(int context, int effect, int messageId, String message, String[] replyContainer);
    public static native int headPoseEstimate(int context, OFL_FaceData faceData);
    public static native int resetHeadPoseEstimate(int context);

    static {
        System.loadLibrary("of_pose");
        System.loadLibrary("oflite");
    }
}
