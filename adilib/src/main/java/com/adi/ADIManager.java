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

import static com.adi.JVMTIConstant.JVMTI_EVENT_GARBAGE_COLLECTION_FINISH;
import static com.adi.JVMTIConstant.JVMTI_EVENT_GARBAGE_COLLECTION_START;
import static com.adi.JVMTIConstant.JVMTI_EVENT_MONITOR_CONTENDED_ENTER;
import static com.adi.JVMTIConstant.JVMTI_EVENT_MONITOR_CONTENDED_ENTERED;
import static com.adi.JVMTIConstant.JVMTI_EVENT_OBJECT_FREE;
import static com.adi.JVMTIConstant.JVMTI_EVENT_THREAD_START;
import static com.adi.JVMTIConstant.JVMTI_EVENT_VM_OBJECT_ALLOC;

/**
 * @author zhoukewen
 * @since 2019-08-12
 */
public class ADIManager {

    private static final String TAG = "ADIManager";

    private static final String LIB_NAME = "adi_agent";

    private static boolean isInited = false;

    private static ADIConfig adiConfig = null;

    // jni 层用到
    private static String packageCodePath = "";

    @SuppressLint("UnsafeDynamicallyLoadedCode")
    public static void init(Context context) {
        if (isInited) {
            return;
        }
        String path = createDuplicateLib(context);
        System.load(path);
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

    /**
     * 启动 Dumper，并开启 JVMTI 事件监听
     *
     * @param context
     * @param config
     */
    public static void start(Context context, ADIConfig config) {
        adiConfig = config;
        File file = context.getExternalCacheDir();
        File root = new File(file.getAbsolutePath(), "ADI/");
        Log.i(TAG, root.getAbsolutePath());
        root.mkdirs();
        startDump(root.getAbsolutePath());

        enableEvents(adiConfig, adiConfig.getEvents());
    }

    /**
     * 停止 Dumper 线程，并关闭 JVMTI 事件监听
     */
    public static void stop() {
        stopDump();
        disableEvents(adiConfig.getEvents());
    }

    public static long getObjSize(Object obj) {
        return getObjectSize(obj);
    }

    /**
     * 启动 Dumper 线程，开始保存 Dumper 文件
     *
     * @param dumpDir
     */
    private static native void startDump(String dumpDir);

    /**
     * 停止 Dumper 线程
     */
    private static native void stopDump();

    /**
     * 开启 JVMTI 的事件监听
     *
     * @param events 事件集合，参考 {@link JVMTIConstant} 中的常量
     */
    private static native void enableEvents(ADIConfig config, int... events);

    /**
     * 停止 JVMTI 的事件监听
     *
     * @param events 事件集合，参考 {@link JVMTIConstant} 中的常量
     */
    private static native void disableEvents(int... events);

    private static native long getObjectSize(Object obj);

    //===============用于 Looper 的测试方法 Start=============

    private static boolean isLoaded = false;

    private static void startTest() {
        if (!isLoaded) {
            System.loadLibrary(LIB_NAME);
            isLoaded = true;
        }
        startLooperForTest();
    }

    private static native void startLooperForTest();

    private static native void pushToLooperForTest(String data);

    private static native void stopLooperForTest();

    //===============用于 Looper 的测试方法 End=============


}
