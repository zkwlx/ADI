package com.adi.demo

import android.util.Log

/**
 * @author zhoukewen
 * @since 2019-07-30
 */
class DemoObject {
    var a = 0
    var b = ""

    companion object {
        @JvmStatic
        fun test() {
            Log.i("zkw", "------>>>>>so nice!!!!")
        }
    }

}
