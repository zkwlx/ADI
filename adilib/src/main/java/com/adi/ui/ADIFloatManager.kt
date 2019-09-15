package com.adi.ui

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.PixelFormat
import android.provider.Settings
import android.view.Gravity
import android.view.ViewGroup
import android.view.WindowManager
import com.adi.R


/**
 * @author zhoukewen
 * @since 2019-09-12
 */
object ADIFloatManager {

    fun showADIFloat(activity: Activity) {
        if (!Settings.canDrawOverlays(activity)) {
            val intent = Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION)
            activity.startActivity(intent)
        } else {
//            val mWindowManager =
//                activity.getSystemService(Context.WINDOW_SERVICE) as WindowManager//获得WindowManager对象
//            val mParams = WindowManager.LayoutParams()
//            val mFloatLayout = FloatLayout(activity, null)//可拖动的View
//            mParams.gravity = Gravity.TOP or Gravity.LEFT//调整悬浮窗显示的停靠位置为左侧置顶
//            mParams.x = 0
//            mParams.y = 100//偏移量
//            mParams.type = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY//总是出现在应用程序窗口之上
//            mParams.format = PixelFormat.RGBA_8888//设置图片格式，效果为背景透明
//            mParams.flags =
//                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE or WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL or
//                        WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN or WindowManager.LayoutParams.FLAG_LAYOUT_INSET_DECOR or
//                        WindowManager.LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH//设置window属性
//            mParams.width = ViewGroup.LayoutParams.WRAP_CONTENT
//            mParams.height = ViewGroup.LayoutParams.WRAP_CONTENT
//            mWindowManager.addView(mFloatLayout, mParams)
            val f = FloatWindow(activity)
            f.setLayout(R.layout.adi_float)
            f.show()
        }

    }


    private const val PERMISSIONS_REQUEST_CODE = 10086
    private val PERMISSIONS = arrayOf(
        "android.permission.SYSTEM_ALERT_WINDOW"
    )


    fun verifyAllPermissions(activity: Activity) {
        val tempPermissions = mutableListOf<String>()
        PERMISSIONS.forEach { permission ->
            val result = activity.checkSelfPermission(permission)
            if (result != PackageManager.PERMISSION_GRANTED) {
                tempPermissions.add(permission)
            }
        }
        activity.requestPermissions(tempPermissions.toTypedArray(), PERMISSIONS_REQUEST_CODE)

    }

}