package com.adi;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Build;
import android.os.Debug;
import android.util.Log;

import java.io.File;
import java.lang.reflect.Method;
import java.nio.file.Files;
import java.nio.file.Paths;

/**
 * @author zhoukewen
 * @since 2019-08-12
 */
public class ADIHelper {

    private static final String TAG = "ADIHelper";

    private static final String LIB_NAME = "adi_agent";

    private static boolean isInited = false;

    // jni 层用到
    private static String packageCodePath = "";

    @SuppressLint("UnsafeDynamicallyLoadedCode")
    public static void init(Context context) {
        if (isInited) {
            return;
        }
        File file = context.getExternalCacheDir();
        File root = new File(file.getAbsolutePath(), "ADI/");
        Log.i(TAG, root.getAbsolutePath());
        boolean result = root.mkdirs();
        String path = createDuplicateLib(context);

        System.load(path);
        startDump(root.getAbsolutePath());

        attachJvmtiAgent(path, context.getClassLoader());

        isInited = true;
    }

    private static void attachJvmtiAgent(String agentPath, ClassLoader classLoader) {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                Debug.attachJvmtiAgent(agentPath, null, classLoader);
            } else {
                Class vmDebugClazz = Class.forName("dalvik.system.VMDebug");
                Method attachAgentMethod = vmDebugClazz.getMethod("attachAgent", String.class);
                attachAgentMethod.setAccessible(true);
                attachAgentMethod.invoke(null, agentPath);
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    private static String createDuplicateLib(Context context) {
        // Build.VERSION.SDK_INT >= Build.VERSION_CODES.O
        try {
            packageCodePath = context.getPackageCodePath();
            ClassLoader classLoader = context.getClassLoader();
            Method findLibrary = ClassLoader.class.getDeclaredMethod("findLibrary", String.class);
            String jvmtiAgentLibPath = (String) findLibrary.invoke(classLoader, LIB_NAME);

            //copy lib to /data/user/0/com.adi.demo/files/adi/agent.so
            Log.d(TAG, "adi_agentlibpath " + jvmtiAgentLibPath);
            File filesDir = context.getFilesDir();
            File jvmtiLibDir = new File(filesDir, "adi");
            if (!jvmtiLibDir.exists()) {
                jvmtiLibDir.mkdirs();
            }
            File agentLibSo = new File(jvmtiLibDir, "agent.so");
            if (agentLibSo.exists()) {
                agentLibSo.delete();
            }
            Files.copy(Paths.get(new File(jvmtiAgentLibPath).getAbsolutePath()), Paths.get((agentLibSo).getAbsolutePath()));

            Log.d(TAG, agentLibSo.getAbsolutePath() + ", " + packageCodePath);
            return agentLibSo.getAbsolutePath();
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    public static void stop() {
        stopDump();
    }

    private static native void startDump(String dumpDir);

    private static native void stopDump();

    //===============用于 Looper 的测试方法 Start=============

    private static boolean isLoaded = false;

    public static void startTest() {
        if (!isLoaded) {
            System.loadLibrary(LIB_NAME);
            isLoaded = true;
        }
        startLooperForTest();
    }

    private static native void startLooperForTest();

    public static native void pushToLooperForTest(String data);

    public static native void stopLooperForTest();

    //===============用于 Looper 的测试方法 End=============


}
