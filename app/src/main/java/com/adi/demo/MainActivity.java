package com.adi.demo;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import com.adi.ADIHelper;

import java.util.ArrayList;

public class MainActivity extends Activity {

    private ArrayList<Object> list = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button tv = findViewById(R.id.sample_text);
        tv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ADIHelper.init(MainActivity.this);
            }
        });
        findViewById(R.id.button_gc).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                System.gc();
                System.runFinalization();
            }
        });
        findViewById(R.id.button_modify_class).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ADIHelper.retransformClasses(new Class[]{Activity.class});
            }
        });
        findViewById(R.id.button_start_activity).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, Main2Activity.class));
            }
        });
        findViewById(R.id.malloc_object).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                for (int i = 0; i < 3; i++) {
                    list.add(new DemoObject());
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
        findViewById(R.id.call_system_service).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                for (int i = 0; i < 5; i++) {
                    String ssid = getWifiSSID();
                    Log.i("zkw", i + ": ssid: " + ssid);
                }
            }
        });
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

