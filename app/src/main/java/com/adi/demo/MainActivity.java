package com.adi.demo;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.util.Log;

import com.adi.ADIManager;
import com.adi.ui.ADIFloatManager;

import java.util.ArrayList;

public class MainActivity extends Activity {

    private ArrayList<Object> list = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.adi_start).setOnClickListener(v -> ADIFloatManager.INSTANCE.showADIFloat(this));
        findViewById(R.id.button_gc).setOnClickListener(v -> {
            System.gc();
            System.runFinalization();
        });
        findViewById(R.id.malloc_object).setOnClickListener(v -> {
            list.add(new DemoObject());
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            Object o = new DemoObject();
            Log.i("adi", "=======+>" + Thread.currentThread().getName() + ": " + Thread.currentThread().getId());
        });
        findViewById(R.id.monitor_test).setOnClickListener(v -> {
            Object a = new Object();
            Object b = new Object();
            Thread t1 = new Thread(() -> {
                synchronized (a) {
                    try {
                        Thread.sleep(3000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    synchronized (b) {
                        try {
                            Thread.sleep(1000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                }
            });
            t1.setName("monitor_test_thread1");
            t1.start();
            Thread t2 = new Thread(() -> {
                synchronized (b) {
                    try {
                        Thread.sleep(3000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    synchronized (a) {
                        try {
                            Thread.sleep(1000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                }

            });
            t2.setName("monitor_test_thread2");
            t2.start();
        });
        findViewById(R.id.call_system_service).setOnClickListener(v -> {
            for (int i = 0; i < 5; i++) {
                String ssid = getWifiSSID();
                Log.i("zkw", i + ": ssid: " + ssid);
            }
        });

        findViewById(R.id.object_size).setOnClickListener(v -> {
            TestObject o = new TestObject();
            long size = ADIManager.getObjSize(o);
            Log.i("zkw", "============>>" + size);
        });
        findViewById(R.id.thread_start).setOnClickListener(v -> {
            for (int i = 0; i < 5; i++) {
                new Thread("TTT_" + i).start();
            }
        });
        //===============用于 Looper 的测试方法 =============
        findViewById(R.id.button_start_looper_test).setOnClickListener(v -> {
            ADIManager.startTest();

            startPushToLooperForTest();
        });
        findViewById(R.id.button_stop_looper_test).setOnClickListener(v -> {
            ADIManager.stopLooperForTest();
//                onRequest();
        });
    }

    private void startPushToLooperForTest() {
        Thread t1 = new Thread(() -> {
            for (int i = 0; i < 5; i++) {
                try {
                    Thread.sleep(50);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                ADIManager.pushToLooperForTest(Thread.currentThread().getName() + ": " + i);
            }
        });
        t1.setName("Thread_1");
        t1.start();

        Thread t2 = new Thread(() -> {
            for (int i = 0; i < 5; i++) {
                try {
                    Thread.sleep(10);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                ADIManager.pushToLooperForTest(Thread.currentThread().getName() + ": " + i);
            }
        });
        t2.setName("Thread_2");
        t2.start();

        Thread t3 = new Thread(() -> {
            for (int i = 0; i < 5; i++) {
                try {
                    Thread.sleep(30);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                ADIManager.pushToLooperForTest(Thread.currentThread().getName() + ": " + i);
            }
        });
        t3.setName("Thread_3");
        t3.start();

    }

    /**
     * 返回当前 Wifi 的 SSID
     * 返回 null 则说明未连接 Wifi 或获取失败
     */
    @SuppressLint("WifiManagerLeak")
    private String getWifiSSID() {
        WifiManager wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        if (wifiManager != null && wifiManager.isWifiEnabled()) {
            try {
                WifiInfo wifiInfo = wifiManager.getConnectionInfo();
                if (wifiInfo != null) {
                    final NetworkInfo.DetailedState state = WifiInfo.getDetailedStateOf(wifiInfo.getSupplicantState());
                    if (state == NetworkInfo.DetailedState.CONNECTED || state == NetworkInfo.DetailedState.OBTAINING_IPADDR) {
                        return wifiInfo.getSSID();
                    }
                }
            } catch (Exception ignored) {
            }
        }
        return null;
    }

}

