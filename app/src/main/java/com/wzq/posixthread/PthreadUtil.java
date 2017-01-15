package com.wzq.posixthread;

/**
 * Created by wzq on 17-1-13.
 */

public class PthreadUtil {


    private MainActivity mainActivity;

    public PthreadUtil(MainActivity mainActivity)
    {
        this.mainActivity = mainActivity;
    }

    static {
        System.loadLibrary("native-lib");
    }


    public native void nativeInit();


    public native void nativeFree();


    public native void nativeWorker(int id, int iterations);

    public native void posixThreads(int id, int iterations);


    public void onNativeMessage(final String message)
    {
        mainActivity.onNativeMessage(message);
    }
}
