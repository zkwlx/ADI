package com.adi.demo

import android.annotation.SuppressLint
import android.app.Activity
import android.content.Context
import android.net.NetworkInfo
import android.net.wifi.WifiInfo
import android.net.wifi.WifiManager
import android.os.Bundle
import android.util.Log
import android.view.View

import com.adi.ADIManager
import com.adi.ui.ADIFloatManager

import java.util.ArrayList
import kotlin.random.Random
import kotlin.random.nextInt

class MainActivity : Activity() {

    private val list = ArrayList<Any>()

    /**
     * 返回当前 Wifi 的 SSID
     * 返回 null 则说明未连接 Wifi 或获取失败
     */
    private val wifiSSID: String?
        @SuppressLint("WifiManagerLeak")
        get() {
            val wifiManager = getSystemService(Context.WIFI_SERVICE) as WifiManager
            if (wifiManager.isWifiEnabled) {
                try {
                    val wifiInfo = wifiManager.connectionInfo
                    if (wifiInfo != null) {
                        val state = WifiInfo.getDetailedStateOf(wifiInfo.supplicantState)
                        if (state == NetworkInfo.DetailedState.CONNECTED || state == NetworkInfo.DetailedState.OBTAINING_IPADDR) {
                            return wifiInfo.ssid
                        }
                    }
                } catch (ignored: Exception) {
                }

            }
            return null
        }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        findViewById<View>(R.id.adi_start).setOnClickListener {
            ADIFloatManager.showADIFloat(this)
        }
        findViewById<View>(R.id.button_gc).setOnClickListener {
            System.gc()
            System.runFinalization()
        }
        findViewById<View>(R.id.malloc_object).setOnClickListener {
            repeat(100) {
                val time = Random.nextInt(10)
                Thread.sleep(time.toLong())
                list.add(DemoObject())
            }
        }
        findViewById<View>(R.id.monitor_test).setOnClickListener {
            testMonitorContend(true)
            Thread.sleep(1000)
            testMonitorContend(false)
        }
        findViewById<View>(R.id.call_system_service).setOnClickListener {
            for (i in 0..4) {
                val ssid = wifiSSID
                Log.i("zkw", "$i: ssid: $ssid")
            }
        }

        findViewById<View>(R.id.object_size).setOnClickListener {
            val o = TestObject()
            val size = ADIManager.getObjSize(o)
            Log.i("zkw", "============>>$size")
        }
        findViewById<View>(R.id.thread_start).setOnClickListener {
            for (i in 0..4) {
                val t = Thread("TTT_$i")
            }
        }
        //===============用于 Looper 的测试方法 =============
        findViewById<View>(R.id.button_start_looper_test).setOnClickListener {
            //            ADIManager.startTest();

            startPushToLooperForTest()
        }
        findViewById<View>(R.id.button_stop_looper_test).setOnClickListener {
            //            ADIManager.stopLooperForTest();
        }
    }

    private fun startPushToLooperForTest() {
        val t1 = Thread {
            for (i in 0..4) {
                Thread.sleep(50)
                //                ADIManager.pushToLooperForTest(Thread.currentThread().getName() + ": " + i);
            }
        }
        t1.name = "Thread_1"
        t1.start()

        val t2 = Thread {
            for (i in 0..4) {
                Thread.sleep(10)
                //                ADIManager.pushToLooperForTest(Thread.currentThread().getName() + ": " + i);
            }
        }
        t2.name = "Thread_2"
        t2.start()

        val t3 = Thread {
            for (i in 0..4) {
                Thread.sleep(30)
                //                ADIManager.pushToLooperForTest(Thread.currentThread().getName() + ": " + i);
            }
        }
        t3.name = "Thread_3"
        t3.start()

    }

    private fun testMonitorContend(isDeadlock: Boolean) {
        val threadNamePrefix = if (isDeadlock) "monitor_deadlock_thread" else "monitor_thread"
        val a = Any()
        val b = Any()
        val t1 = Thread {
            synchronized(a) {
                val z = Any()
                synchronized(z) {
                    Thread.sleep(300)
                    val k = Any()
                    synchronized(k) {
                        if (isDeadlock) {
                            synchronized(b) {
                                Thread.sleep(100)
                            }
                        }
                    }
                }
            }
        }
        t1.name = threadNamePrefix + "1"
        t1.start()
        val t2 = Thread {
            synchronized(b) {
                val z = Any()
                synchronized(z) {
                    Thread.sleep(100)
                    val k = Any()
                    synchronized(k) {
                        Log.i("", "")
                        synchronized(a) {
                            Thread.sleep(100)
                        }
                    }
                }
            }

        }
        t2.name = threadNamePrefix + "2"
        t2.start()
    }

}

