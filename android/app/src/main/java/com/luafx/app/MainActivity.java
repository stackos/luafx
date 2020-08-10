package com.luafx.app;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.opengl.GLES20;
import android.os.Build;
import android.os.Bundle;
import android.app.AlertDialog;
import android.util.Log;
import android.view.WindowManager;
import android.widget.RelativeLayout;
import android.widget.Toast;

import com.luafx.app.utils.GLView;
import com.luafx.app.utils.GLTexture;
import com.luafx.lib.LuaFX;

import java.io.File;

public class MainActivity extends Activity {
    private static final String TAG = "MainActivity";

    private static final int PERMISSION_REQUEST_CAMERA = 0;
    private static final int PERMISSION_REQUEST_STORAGE = 1;

    private RelativeLayout mLayout;
    private GLView mGLView;
    private AlertDialog mDialog;
    private int mFrameCount = 0;
    private long mFrameTime = 0;

    private int mContext = LuaFX.LFX_INVALID_HANDLE;
    private int mEffect = LuaFX.LFX_INVALID_HANDLE;
    private LuaFX.LFX_Texture mTextureIn = new LuaFX.LFX_Texture();
    private LuaFX.LFX_Texture mTextureOut = new LuaFX.LFX_Texture();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        this.checkPermissions();
    }

    private void checkPermissions() {
        if (Build.VERSION.SDK_INT >= 23) {
            if (this.checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                this.requestPermissions(
                    new String[]{
                        Manifest.permission.WRITE_EXTERNAL_STORAGE,
                        Manifest.permission.READ_EXTERNAL_STORAGE
                    },
                    PERMISSION_REQUEST_STORAGE
                );
            } else if (this.checkSelfPermission(Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
                this.requestPermissions(new String[]{Manifest.permission.CAMERA}, PERMISSION_REQUEST_CAMERA);
            } else {
                this.setCameraView();
            }
        } else {
            this.setCameraView();
        }
    }

    @Override
    public void onRequestPermissionsResult(
            int requestCode,
            String[] permissions,
            int[] grantResults) {
        if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            this.checkPermissions();
        } else {
            Toast.makeText(this, "所需权限被拒绝", Toast.LENGTH_LONG).show();
        }
    }

    @Override
    public void onResume() {
        super.onResume();

        if (mGLView != null) {
            mGLView.onResume();
        }
    }

    @Override
    public void onPause() {
        if (mGLView != null) {
            mGLView.onPause();
        }
        super.onPause();
    }

    private void extractAssetsDirs(String[] dirs) {
        for (int i = 0; i < dirs.length; ++i) {
            String dstDir = getFilesDir().getPath() + "/" + dirs[i];
            File dstDirFile = new File(dstDir);
            if (!dstDirFile.exists()) {
                dstDirFile.mkdirs();
            }
            LuaFX.extractAssetsDir(getAssets(), dirs[i], dstDir);
        }
    }

    private void setCameraView() {
        extractAssetsDirs(new String[] { "assets", "assets/demo", "assets/input", "assets/lua_lib", "assets/font" });

        mLayout = new RelativeLayout(this);
        mGLView = new GLView(this);

        // for render
        mGLView.setDrawFrameCallback(new GLView.DrawFrameCallback() {
            @Override
            public void onDrawFrame(GLTexture textureOut) {
                mTextureOut.id = textureOut.getTextureId();
                mTextureOut.target = textureOut.getTarget();
                mTextureOut.format = textureOut.getFormat();
                mTextureOut.width = textureOut.getWidth();
                mTextureOut.height = textureOut.getHeight();
                mTextureOut.filterMode = GLES20.GL_LINEAR;
                mTextureOut.wrapMode = GLES20.GL_CLAMP_TO_EDGE;

                LuaFX.renderEffect(mContext, mEffect, mTextureIn, mTextureOut, null);

                mFrameCount++;
                long now = System.currentTimeMillis();
                if (now - mFrameTime > 1000) {
                    int fps = mFrameCount;
                    mFrameCount = 0;
                    mFrameTime = now;

                    Log.w(TAG, "fps: " + fps + " w:" + textureOut.getWidth() + " h:" + textureOut.getHeight());
                }
            }

            @Override
            public void onInit() {
                Log.i(TAG, "onInit");

                int[] arr = new int[1];
                int ret = LuaFX.createContext(arr);
                if (ret != LuaFX.LFX_SUCCESS) {
                    Log.e(TAG, "luafx error: " + ret);
                    return;
                }

                mContext = arr[0];

                String effectPath = getFilesDir().getPath() + "/assets/effect.lua";
                ret = LuaFX.loadEffect(mContext, effectPath, arr);
                if (ret != LuaFX.LFX_SUCCESS) {
                    Log.e(TAG, "luafx error: " + ret);
                    return;
                }

                mEffect = arr[0];

                String imagePath = getFilesDir().getPath() + "/assets/input/720x1280.png";
                LuaFX.loadTexture2D(mContext, imagePath, mTextureIn);
            }

            @Override
            public void onRelease() {
                Log.i(TAG, "onRelease");

                if (mDialog != null) {
                    mDialog.dismiss();
                    mDialog = null;
                }

                if (mEffect != LuaFX.LFX_INVALID_HANDLE) {
                    LuaFX.destroyEffect(mContext, mEffect);
                    mEffect = LuaFX.LFX_INVALID_HANDLE;
                }

                if (mContext != LuaFX.LFX_INVALID_HANDLE) {
                    LuaFX.destroyContext(mContext);
                    mContext = LuaFX.LFX_INVALID_HANDLE;
                }
            }
        });

        mLayout.addView(
                mGLView,
                new RelativeLayout.LayoutParams(
                    RelativeLayout.LayoutParams.MATCH_PARENT,
                    RelativeLayout.LayoutParams.MATCH_PARENT
                )
        );

        this.setContentView(mLayout);
    }
}
