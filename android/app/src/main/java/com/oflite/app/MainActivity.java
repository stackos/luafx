package com.oflite.app;

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

import com.oflite.app.utils.CameraUtil;
import com.oflite.app.utils.CameraView;
import com.oflite.app.utils.GLTexture;
import com.oflite.lib.OFLite;

import java.io.File;

public class MainActivity extends Activity {
    private static final String TAG = "MainActivity";

    private static final int PERMISSION_REQUEST_CAMERA = 0;
    private static final int PERMISSION_REQUEST_STORAGE = 1;

    private RelativeLayout mLayout;
    private CameraView mCameraView;
    private AlertDialog mDialog;
    private int mFrameCount = 0;
    private long mFrameTime = 0;

    private int mOFLContext = OFLite.OFL_INVALID_HANDLE;
    private int mEffect = OFLite.OFL_INVALID_HANDLE;
    private OFLite.OFL_Texture mTextureIn = new OFLite.OFL_Texture();
    private OFLite.OFL_Texture mTextureOut = new OFLite.OFL_Texture();

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

        if (mCameraView != null) {
            mCameraView.onResume();
        }
    }

    @Override
    public void onPause() {
        if (mCameraView != null) {
            mCameraView.onPause();
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
            OFLite.extractAssetsDir(getAssets(), dirs[i], dstDir);
        }
    }

    private void setCameraView() {
        extractAssetsDirs(new String[] { "assets", "assets/sticker" });

        mLayout = new RelativeLayout(this);
        mCameraView = new CameraView(this);

        // for render
        mCameraView.setDrawFrameCallback(720, 1280, new CameraView.DrawFrameCallback() {
            @Override
            public void onDrawFrame(GLTexture textureIn, GLTexture textureOut, CameraUtil.ReadedImage image) {
                if (image.data == null) {
                    mCameraView.copyTexture(textureIn, textureOut);
                    return;
                }

                if (mEffect != OFLite.OFL_INVALID_HANDLE) {
                    mTextureIn.id = textureIn.getTextureId();
                    mTextureIn.target = textureIn.getTarget();
                    mTextureIn.format = textureIn.getFormat();
                    mTextureIn.width = textureIn.getWidth();
                    mTextureIn.height = textureIn.getHeight();
                    mTextureIn.filterMode = GLES20.GL_LINEAR;
                    mTextureIn.wrapMode = GLES20.GL_CLAMP_TO_EDGE;

                    mTextureOut.id = textureOut.getTextureId();
                    mTextureOut.target = textureOut.getTarget();
                    mTextureOut.format = textureOut.getFormat();
                    mTextureOut.width = textureOut.getWidth();
                    mTextureOut.height = textureOut.getHeight();
                    mTextureOut.filterMode = GLES20.GL_LINEAR;
                    mTextureOut.wrapMode = GLES20.GL_CLAMP_TO_EDGE;

                    OFLite.OFL_FaceData faceData = new OFLite.OFL_FaceData();
                    faceData.faceCount = 1;
                    OFLite.headPoseEstimate(mOFLContext, faceData);

                    OFLite.renderEffect(mOFLContext, mEffect, mTextureIn, mTextureOut, null);
                } else {
                    mCameraView.copyTexture(textureIn, textureOut);
                }

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
                int ret = OFLite.createContext(arr);
                if (ret != OFLite.OFL_SUCCESS) {
                    Log.e(TAG, "ofl error: " + ret);
                    return;
                }

                mOFLContext = arr[0];

                String effectPath = getFilesDir().getPath() + "/assets/effect.ofeffect";
                ret = OFLite.loadEffect(mOFLContext, effectPath, arr);
                if (ret != OFLite.OFL_SUCCESS) {
                    Log.e(TAG, "ofl error: " + ret);
                    return;
                }

                mEffect = arr[0];
            }

            @Override
            public void onRelease() {
                Log.i(TAG, "onRelease");

                if (mDialog != null) {
                    mDialog.dismiss();
                    mDialog = null;
                }

                if (mEffect != OFLite.OFL_INVALID_HANDLE) {
                    OFLite.destroyEffect(mOFLContext, mEffect);
                    mEffect = OFLite.OFL_INVALID_HANDLE;
                }

                if (mOFLContext != OFLite.OFL_INVALID_HANDLE) {
                    OFLite.destroyContext(mOFLContext);
                    mOFLContext = OFLite.OFL_INVALID_HANDLE;
                }
            }
        });

        mLayout.addView(
                mCameraView,
                new RelativeLayout.LayoutParams(
                    RelativeLayout.LayoutParams.MATCH_PARENT,
                    RelativeLayout.LayoutParams.MATCH_PARENT
                )
        );

        this.setContentView(mLayout);
    }
}
