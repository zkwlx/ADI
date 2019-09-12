package com.adi.ui

import android.app.Activity
import android.content.Intent
import android.content.pm.PackageManager
import android.provider.Settings
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
            val window = FloatWindow(activity)
            window.setLayout(R.layout.adi_float)
            window.show()
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