package com.luafx.app.utils;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.util.AttributeSet;
import android.util.Log;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class GLView extends GLSurfaceView implements GLSurfaceView.Renderer {
    private static final String TAG = "GLView";

    private QuadRenderer mQuadRenderer;
    private DrawFrameCallback mDrawFrameCallback;
    private int mViewWidth;
    private int mViewHeight;
    private GLTexture mTextureOut;
    private float[] mIdentityMatrix;
    private float[] mFlipYMatrix;
    private static int mGLContextVersion = 0;

    public GLView(Context context) {
        this(context, null);
    }

    public GLView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser {
        private static final int EGL_OPENGL_ES2_BIT  = 0x0004;
        private static final int EGL_OPENGL_ES3_BIT  = 0x0040;

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
            Log.i(TAG, "egl chooseConfig");

            int[] attribs = {
                EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                EGL10.EGL_RED_SIZE, 8,
                EGL10.EGL_GREEN_SIZE, 8,
                EGL10.EGL_BLUE_SIZE, 8,
                EGL10.EGL_NONE
            };

            int[] countArray = new int[1];
            if (egl.eglGetConfigs(display, null, 0, countArray)) {
                int count = countArray[0];
                if (count > 0) {
                    EGLConfig[] configs = new EGLConfig[count];

                    // try es3
                    boolean success = egl.eglChooseConfig(display, attribs, configs, configs.length, countArray);
                    int err = egl.eglGetError();
                    if (err != EGL10.EGL_SUCCESS) {
                        Log.e(TAG, "egl chooseConfig error for es3: " + err);
                    }
                    if (success && countArray[0] > 0) {
                        Log.w(TAG, "egl chooseConfig: es3");
                        return configs[0];
                    }

                    // es2
                    attribs[1] = EGL_OPENGL_ES2_BIT;
                    success = egl.eglChooseConfig(display, attribs, configs, configs.length, countArray);
                    err = egl.eglGetError();
                    if (err != EGL10.EGL_SUCCESS) {
                        Log.e(TAG, "egl chooseConfig error for es2: " + err);
                    }
                    if (success && countArray[0] > 0) {
                        Log.w(TAG, "egl chooseConfig: es2");
                        return configs[0];
                    }
                }
            }

            Log.e(TAG, "egl chooseConfig: none");
            return null;
        }
    }

    private static class ContextFactory implements GLSurfaceView.EGLContextFactory {
        private static final int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

        public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig config) {
            Log.i(TAG, "egl createContext");

            int[] attribs = {
                EGL_CONTEXT_CLIENT_VERSION, 3,
                EGL10.EGL_NONE
            };

            // es3
            EGLContext context = egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT, attribs);
            int err = egl.eglGetError();
            if (err != EGL10.EGL_SUCCESS) {
                Log.e(TAG, "egl createContext error for es3: " + err);
            }
            if (context != null && context != EGL10.EGL_NO_CONTEXT) {
                mGLContextVersion = 3;
                Log.w(TAG, "egl createContext: es3");
                return context;
            }

            // es2
            attribs[1] = 2;
            context = egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT, attribs);
            err = egl.eglGetError();
            if (err != EGL10.EGL_SUCCESS) {
                Log.e(TAG, "egl createContext error for es2: " + err);
            }
            if (context != null && context != EGL10.EGL_NO_CONTEXT) {
                mGLContextVersion = 2;
                Log.w(TAG, "egl createContext: es2");
                return context;
            }

            Log.e(TAG, "egl createContext: none");
            return null;
        }

        public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
            egl.eglDestroyContext(display, context);
        }
    }

    private void init() {
        mIdentityMatrix = new float[16];
        Matrix.setIdentityM(mIdentityMatrix, 0);

        mFlipYMatrix = new float[16];
        Matrix.setIdentityM(mFlipYMatrix, 0);
        Matrix.scaleM(mFlipYMatrix, 0, 1f, -1, 1);

        setEGLConfigChooser(new ConfigChooser());
        setEGLContextFactory(new ContextFactory());
        setPreserveEGLContextOnPause(true);
        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }

    private void release() {
        Log.d(TAG, "release");

        if (mDrawFrameCallback != null) {
            mDrawFrameCallback.onRelease();
        }

        if (mQuadRenderer != null) {
            mQuadRenderer.release();
            mQuadRenderer = null;
        }

        if (mTextureOut != null) {
            mTextureOut.release();
            mTextureOut = null;
        }
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onPause() {
        queueEvent(new Runnable() {
            @Override
            public void run() {
                release();
            }
        });

        super.onPause();
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        Log.w(TAG, "onSurfaceCreated GLES Version: " + GLES20.glGetString(GLES20.GL_VERSION));
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int w, int h) {
        Log.w(TAG, "onSurfaceChanged: " + w + " " + h);

        mViewWidth = w;
        mViewHeight = h;

        if (mQuadRenderer == null) {
            mQuadRenderer = new QuadRenderer(false);

            if (mDrawFrameCallback != null) {
                mDrawFrameCallback.onInit();
            }
        }
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        if (mDrawFrameCallback != null) {
            if (mTextureOut == null || mTextureOut.getWidth() != mViewWidth || mTextureOut.getHeight() != mViewHeight) {
                if (mTextureOut != null) {
                    mTextureOut.release();
                    mTextureOut = null;
                }

                mTextureOut = new GLTexture(GLES20.GL_TEXTURE_2D);
                mTextureOut.texImage(mViewWidth, mViewHeight);
            }

            mDrawFrameCallback.onDrawFrame(mTextureOut);

            mQuadRenderer.draw(mIdentityMatrix, mTextureOut.getTextureId());
        }
    }

    public interface DrawFrameCallback {
        void onDrawFrame(GLTexture textureOut);

        void onInit();

        void onRelease();
    }

    public void setDrawFrameCallback(DrawFrameCallback callback) {
        mDrawFrameCallback = callback;
    }
}
