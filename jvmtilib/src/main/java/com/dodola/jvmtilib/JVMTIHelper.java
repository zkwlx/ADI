package com.dodola.jvmtilib;

import android.app.Activity;
import android.content.Context;
import android.os.Build;
import android.os.Debug;
import android.util.Log;
import android.widget.Toast;
import android.os.Process;

import java.io.File;
import java.lang.reflect.Method;

import java.nio.file.Files;
import java.nio.file.Paths;

/**
 * Created by dodola on 2018/12/16.
 */
public class JVMTIHelper {
    private static String packageCodePath = "";

    public static void init(Context context) {

//        if (true) {
//            System.loadLibrary("jvmti_agent");
//            return;
//        }

        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                packageCodePath = context.getPackageCodePath();
                ClassLoader classLoader = context.getClassLoader();
                Method findLibrary = ClassLoader.class.getDeclaredMethod("findLibrary", String.class);
                String jvmtiAgentLibPath = (String) findLibrary.invoke(classLoader, "jvmti_agent");
                //copy lib to /data/data/com.dodola.jvmti/files/jvmti
                Log.d("jvmtiagentlibpath", "jvmtiagentlibpath " + jvmtiAgentLibPath);
                File filesDir = context.getFilesDir();
                File jvmtiLibDir = new File(filesDir, "jvmti");
                if (!jvmtiLibDir.exists()) {
                    jvmtiLibDir.mkdirs();

                }
                File agentLibSo = new File(jvmtiLibDir, "agent.so");
                if (agentLibSo.exists()) {
                    agentLibSo.delete();
                }
                Files.copy(Paths.get(new File(jvmtiAgentLibPath).getAbsolutePath()), Paths.get((agentLibSo).getAbsolutePath()));

                Log.d("Jvmti", agentLibSo.getAbsolutePath() + "," + context.getPackageCodePath());

                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                    Debug.attachJvmtiAgent(agentLibSo.getAbsolutePath(), null, classLoader);
                } else {
                    try {
                        Class vmDebugClazz = Class.forName("dalvik.system.VMDebug");
                        Method attachAgentMethod = vmDebugClazz.getMethod("attachAgent", String.class);
                        attachAgentMethod.setAccessible(true);
                        attachAgentMethod.invoke(null, agentLibSo.getAbsolutePath());
                    } catch (Exception ex) {
                        ex.printStackTrace();
                    }
                }
                System.loadLibrary("jvmti_agent");
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    public static native void retransformClasses(Class[] classes);

    public static void printStatus(String methodName) {
        int pid = Process.myPid();
        String threadName = Thread.currentThread().getName();
        Log.i("zkw", "---------> pid:" + pid + ", threadName:" + threadName + ", jni fun: " + methodName);
    }

    public static void printEnter(String log) {
        Log.d("JVMTIHelper", "_____________________" + log);
    }

    public static void printEnter(Activity context, String log) {
        Toast.makeText(context, "======" + log, Toast.LENGTH_SHORT).show();
        Log.d("JVMTIHelper", "_____________________" + log);
    }


}
