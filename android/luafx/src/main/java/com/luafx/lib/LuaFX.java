package com.luafx.lib;

import android.content.res.AssetManager;

public class LuaFX {
    public static final int LFX_INVALID_HANDLE = 0;

    // result code
    public static final int LFX_SUCCESS = 0;
    public static final int LFX_FAIL = 1;
    public static final int LFX_INVALID_CONTEXT = 2;
    public static final int LFX_INVALID_INPUT = 3;
    public static final int LFX_IMAGE_LOAD_FAIL = 4;
    public static final int LFX_SHADER_COMPILE_ERROR = 5;
    public static final int LFX_PROGRAM_LINK_ERROR = 6;
    public static final int LFX_GL_ERROR = 7;
    public static final int LFX_INVALID_EFFECT = 8;
    public static final int LFX_FILE_OPEN_FAIL = 9;
    public static final int LFX_FILE_READ_ERROR = 10;
    public static final int LFX_LUA_LOAD_ERROR = 11;
    public static final int LFX_LUA_NO_FUNCTION = 12;
    public static final int LFX_LUA_RUN_ERROR = 13;
    public static final int LFX_INVLAID_GL_CONTEXT = 14;
    public static final int LFX_UNKNOWN_GL_VERSION = 15;
    public static final int LFX_NOT_SUPPORT_GL_VERSION = 16;
    public static final int LFX_MESSAGE_REPLY_BUFFER_NOT_ENOUGH = 17;
    public static final int LFX_JNI_ENV_ERROR = 18;
    public static final int LFX_ASSET_OPEN_FAIL = 19;
    public static final int LFX_NO_IMPLEMENT = 20;

    // message id
    public static final int LFX_MESSAGE_ID_SET_EFFECT_TIMESTAMP = -1;

    public static class LFX_Texture {
        public int id = 0;
        public int target = 0;
        public int format = 0;
        public int width = 0;
        public int height = 0;
        public int filterMode = 0;
        public int wrapMode = 0;
    }

    // message
    public interface LFX_EffectMessageCallback {
        String onRecieve(int messageId, String message);
    }

    private static LFX_EffectMessageCallback mEffectMessageCallback = null;

    private static String effectMessageCallback(int messageId, String message) {
        if (mEffectMessageCallback != null) {
            return mEffectMessageCallback.onRecieve(messageId, message);
        }
        return null;
    }

    public static int setEffectMessageCallback(LFX_EffectMessageCallback callback) {
        mEffectMessageCallback = callback;
        return LFX_SUCCESS;
    }

    // log
    public interface LFX_LogCallback {
        void log(String message);
    }

    private static LFX_LogCallback mLogCallback = null;

    private static void logCallback(String message) {
        if (mLogCallback != null) {
            mLogCallback.log(message);
        }
    }

    public static int setLogCallback(LFX_LogCallback callback) {
        mLogCallback = callback;
        return LFX_SUCCESS;
    }

    // native methods
    public static native int extractAssetsDir(AssetManager assetManager, String srcDir, String dstDir);
    public static native int createContext(int[] contextContainer);
    public static native int destroyContext(int context);
    public static native int loadTexture2D(int context, String path, LFX_Texture texture);
    public static native int createTexture(int context, LFX_Texture texture);
    public static native int destroyTexture(int context, LFX_Texture texture);
    public static native int renderQuad(int context, LFX_Texture texture, float[] matrix);
    public static native int loadEffect(int context, String path, int[] effectContainer);
    public static native int destroyEffect(int context, int effect);
    public static native int renderEffect(int context, int effect, LFX_Texture inputTexture, LFX_Texture outputTexture, byte[] outputImage);
    public static native int sendEffectMessage(int context, int effect, int messageId, String message, String[] replyContainer);

    static {
        System.loadLibrary("luafx");
    }
}
