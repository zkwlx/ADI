package com.adi.demo;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;

import com.adi.ADIHelper;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Proxy;
import java.util.ArrayList;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Connection;
import okhttp3.EventListener;
import okhttp3.Handshake;
import okhttp3.OkHttpClient;
import okhttp3.Protocol;
import okhttp3.Request;
import okhttp3.Response;

public class MainActivity extends Activity {

    private ArrayList<Object> list = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.sample_text).setOnClickListener(v -> ADIHelper.init(MainActivity.this));
        findViewById(R.id.adi_start).setOnClickListener(v -> ADIHelper.start(MainActivity.this));
        findViewById(R.id.adi_stop).setOnClickListener(v -> ADIHelper.stop());

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
        findViewById(R.id.call_system_service).setOnClickListener(v -> {
            for (int i = 0; i < 5; i++) {
                String ssid = getWifiSSID();
                Log.i("zkw", i + ": ssid: " + ssid);
            }
        });

        findViewById(R.id.object_size).setOnClickListener(v -> {
            TestObject o = new TestObject();
            long size = ADIHelper.getObjSize(o);
            Log.i("zkw", "============>>" + size);
        });
        findViewById(R.id.thread_start).setOnClickListener(v -> {
            for (int i = 0; i < 5; i++) {
                new Thread("TTT_" + i).start();
            }
        });
        //===============用于 Looper 的测试方法 =============
        findViewById(R.id.button_start_looper_test).setOnClickListener(v -> {
            ADIHelper.startTest();

            startPushToLooperForTest();
        });
        findViewById(R.id.button_stop_looper_test).setOnClickListener(v -> {
            ADIHelper.stopLooperForTest();
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
                ADIHelper.pushToLooperForTest(Thread.currentThread().getName() + ": " + i);
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
                ADIHelper.pushToLooperForTest(Thread.currentThread().getName() + ": " + i);
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
                ADIHelper.pushToLooperForTest(Thread.currentThread().getName() + ": " + i);
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

    public void onRequest() {
        String url = "http://www.baidu.com";
        OkHttpClient okHttpClient = new OkHttpClient.Builder()
                .eventListener(new MonitorEventListener())
                .build();
        Request request = new Request.Builder()
                .url(url)
                .build();
        Call call = okHttpClient.newCall(request);
        Request request2 = new Request.Builder()
                .url("http://www.zhihu.com")
                .build();
        Call call2 = okHttpClient.newCall(request2);
        call2.enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {

            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {

            }
        });
        try {
            call.enqueue(new Callback() {
                @Override
                public void onFailure(Call call, IOException e) {
                    e.printStackTrace();
                }

                @Override
                public void onResponse(Call call, Response response) throws IOException {
                    System.out.println(response.body().string());
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private class MonitorEventListener extends EventListener {
        @Override
        public void callStart(Call call) {
            super.callStart(call);
            Log.i("zkw", this.toString() + "callStart-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }

        @Override
        public void dnsStart(Call call, String domainName) {
            super.dnsStart(call, domainName);
            Log.i("zkw", this.toString() + "dnsStart-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }

        @Override
        public void dnsEnd(Call call, String domainName, List<InetAddress> inetAddressList) {
            super.dnsEnd(call, domainName, inetAddressList);
            Log.i("zkw", this.toString() + "dnsEnd-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }

        @Override
        public void connectStart(Call call, InetSocketAddress inetSocketAddress, Proxy proxy) {
            super.connectStart(call, inetSocketAddress, proxy);
            Log.i("zkw", "connectStart-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }

        @Override
        public void secureConnectStart(Call call) {
            super.secureConnectStart(call);
            Log.i("zkw", "secureConnectStart-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }

        @Override
        public void secureConnectEnd(Call call, Handshake handshake) {
            super.secureConnectEnd(call, handshake);
            Log.i("zkw", "secureConnectEnd-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }

        @Override
        public void connectEnd(Call call, InetSocketAddress inetSocketAddress, Proxy proxy, Protocol protocol) {
            super.connectEnd(call, inetSocketAddress, proxy, protocol);
            Log.i("zkw", "connectEnd-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }

        @Override
        public void connectFailed(Call call, InetSocketAddress inetSocketAddress, Proxy proxy, Protocol protocol, IOException ioe) {
            super.connectFailed(call, inetSocketAddress, proxy, protocol, ioe);
            Log.i("zkw", "connectFailed-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }

        @Override
        public void connectionAcquired(Call call, Connection connection) {
            super.connectionAcquired(call, connection);
            Log.i("zkw", "connectionAcquired-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }

        @Override
        public void connectionReleased(Call call, Connection connection) {
            super.connectionReleased(call, connection);
            Log.i("zkw", "connectionReleased-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }

        @Override
        public void requestHeadersStart(Call call) {
            super.requestHeadersStart(call);
            Log.i("zkw", "requestHeadersStart-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }

        @Override
        public void responseHeadersStart(Call call) {
            super.responseHeadersStart(call);
            Log.i("zkw", "responseHeadersStart-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }

        @Override
        public void callEnd(Call call) {
            super.callEnd(call);
            Log.i("zkw", this.toString() + "callEnd-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }

        @Override
        public void callFailed(Call call, IOException ioe) {
            super.callFailed(call, ioe);
            Log.i("zkw", this.toString() + "callFailed-> " + call.toString() + ", thread:" + Thread.currentThread().getName());
        }
    }

}

