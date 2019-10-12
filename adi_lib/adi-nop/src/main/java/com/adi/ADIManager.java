package com.adi;

import android.annotation.SuppressLint;
import android.content.Context;

/**
 * @author zhoukewen
 * @since 2019-08-12
 */
public class ADIManager {

    @SuppressLint("UnsafeDynamicallyLoadedCode")
    public static void init(Context context) {
        // no-op
    }

    /**
     * 启动 Dumper，并开启 JVMTI 事件监听
     *
     * @param context
     * @param config
     */
    public static void start(Context context, ADIConfig config) {
        // no-op
    }

    /**
     * 停止 Dumper 线程，并关闭 JVMTI 事件监听
     */
    public static void stop() {
        // no-op
    }

    public static long getObjSize(Object obj) {
        // no-op
        return -1;
    }


}
