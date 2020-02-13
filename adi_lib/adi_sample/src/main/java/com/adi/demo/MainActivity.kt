package com.adi.demo

import android.annotation.SuppressLint
import android.app.Activity
import android.content.Context
import android.net.NetworkInfo
import android.net.wifi.WifiInfo
import android.net.wifi.WifiManager
import android.os.Build
import android.os.Bundle
import android.support.annotation.RequiresApi
import android.util.Log
import android.view.View
import com.adi.ADIManager
import com.adi.ui.ADIFloatManager
import com.tencent.matrix.iocanary.core.IOCanaryCore
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.io.IOException
import java.nio.file.Files
import java.nio.file.Paths
import java.util.*
import kotlin.random.Random


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

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

//        ADIManager.init(this)


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

        findViewById<View>(R.id.retransform_test).setOnClickListener {
            val o = TestObject()
//            ADIManager.retransformClass(arrayOf(TestObject::class.java))
            o.a()
        }
        findViewById<View>(R.id.thread_start).setOnClickListener {
            //            for (i in 0..4) {
//                val t = Thread("TTT_$i")
//            }
            val ooo = TestObject()
            ooo.a()
        }
        //===============用于 Looper 的测试方法 =============
        findViewById<View>(R.id.button_start_looper_test).setOnClickListener {
            //            ADIManager.startTest();

            startPushToLooperForTest()
        }
        findViewById<View>(R.id.button_stop_looper_test).setOnClickListener {
            //            ADIManager.stopLooperForTest();
        }

        findViewById<View>(R.id.io_test).setOnClickListener {
            createDuplicateLib(this)
            val sourceFile = File(File(filesDir, "adi"), "agent.so")
            it.post {
                val destDirt = File(filesDir, "aaaaai")
                if (!destDirt.exists()) {
                    destDirt.mkdirs()
                }
                val destFile = File(destDirt, "agent.sooooo")
                copyFile(sourceFile, destFile)
                Log.i("zkw", "复制完毕。。。。。")
            }
            val bytes = sourceFile.readBytes()
            Log.i("zkw", "bytes------->>${bytes.size}")
        }

    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    private fun createDuplicateLib(context: Context): String? { // Build.VERSION.SDK_INT >= Build.VERSION_CODES.O
        return try {
            val classLoader = context.classLoader
            val findLibrary =
                ClassLoader::class.java.getDeclaredMethod("findLibrary", String::class.java)
            val jvmtiAgentLibPath = findLibrary.invoke(classLoader, "adi_agent") as String
            //copy lib to /data/user/0/com.adi.demo/files/adi/agent.so
            val filesDir = context.filesDir
            val jvmtiLibDir = File(filesDir, "adi")
            if (!jvmtiLibDir.exists()) {
                jvmtiLibDir.mkdirs()
            }
            val agentLibSo = File(jvmtiLibDir, "agent.so")
            if (agentLibSo.exists()) {
                agentLibSo.delete()
            }
            Files.copy(
                Paths.get(File(jvmtiAgentLibPath).absolutePath),
                Paths.get(agentLibSo.absolutePath)
            )
            agentLibSo.absolutePath
        } catch (e: java.lang.Exception) {
            e.printStackTrace()
            null
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

    @Throws(IOException::class)
    private fun copyFile(source: File, dest: File) {
        val input = FileInputStream(source)
        val output = FileOutputStream(dest)
        try {
            val buf = ByteArray(5)
            var bytesRead: Int
            while (input.read(buf).also { bytesRead = it } > 0) {
                output.write(buf, 0, bytesRead)
            }
        } finally {
            input.close()
            output.close()
        }
    }


}

