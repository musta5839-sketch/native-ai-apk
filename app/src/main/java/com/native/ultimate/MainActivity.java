package com.native.ultimate;

import android.app.Activity;
import android.os.Bundle;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.widget.Toast;

public class MainActivity extends Activity implements SurfaceHolder.Callback {
    static {
        System.loadLibrary("ultimate_engine");
    }

    private native void nativeInit();
    private native void nativeShutdown();
    private native void nativeSurfaceCreated(long surface);
    private native void nativeSurfaceChanged(int width, int height);
    private native void nativeSurfaceDestroyed();
    private native void nativeRenderStep();

    private SurfaceView surfaceView;
    private boolean nativeInitialized = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        surfaceView = new SurfaceView(this);
        surfaceView.getHolder().addCallback(this);
        setContentView(surfaceView);
        nativeInit();
        nativeInitialized = true;
        Toast.makeText(this, "FIFA 2025 Native AI Engine Started", Toast.LENGTH_SHORT).show();
    }

    @Override
    protected void onDestroy() {
        if (nativeInitialized) {
            nativeShutdown();
        }
        super.onDestroy();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        nativeSurfaceCreated(holder.getSurface().getNativeHandle());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        nativeSurfaceChanged(width, height);
        new Thread(() -> {
            while (true) {
                nativeRenderStep();
                try {
                    Thread.sleep(16);
                } catch (InterruptedException e) {
                    break;
                }
            }
        }).start();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        nativeSurfaceDestroyed();
    }
}
