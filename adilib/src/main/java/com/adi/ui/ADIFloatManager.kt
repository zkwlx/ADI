package com.adi.ui

import android.app.Activity
import android.content.Intent
import android.content.pm.PackageManager
import android.provider.Settings
import android.text.InputType
import android.text.TextUtils
import android.view.LayoutInflater
import android.view.View
import android.view.WindowManager
import android.widget.Button
import android.widget.EditText
import android.widget.ProgressBar
import com.adi.ADIManager
import com.adi.R


/**
 * @author zhoukewen
 * @since 2019-09-12
 */
object ADIFloatManager {

    fun showADIFloat(activity: Activity) = if (!Settings.canDrawOverlays(activity)) {
        val intent = Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION)
        activity.startActivity(intent)
    } else {
        val f = FloatWindow(activity)
        val view = LayoutInflater.from(activity).inflate(R.layout.adi_float, null)
        val sampleEdit = view.findViewById<EditText>(R.id.sample_edit)
        val adiBtn = view.findViewById<Button>(R.id.adi_btn)
        val progress = view.findViewById<ProgressBar>(R.id.progress_bar)
        val defaultInputType = sampleEdit.inputType
        var started = false
        adiBtn.setOnClickListener {
            if (started) {
                progress.visibility = View.INVISIBLE
                adiBtn.setBackgroundResource(android.R.drawable.ic_media_play)
                sampleEdit.inputType = defaultInputType
                f.wmParams.alpha = 0.6f
                f.wmParams.flags = WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL or
                        WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN or
                        WindowManager.LayoutParams.FLAG_LAYOUT_INSET_DECOR or
                        WindowManager.LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH
                f.updateParams()
                ADIManager.stopForDefaultEvents()
                started = false
            } else {
                progress.visibility = View.VISIBLE
                adiBtn.setBackgroundResource(android.R.drawable.ic_media_pause)
                ADIManager.init(activity)
                var sample = sampleEdit.text.toString()
                if (TextUtils.isEmpty(sample)) {
                    sample = "0.8"
                }
                sampleEdit.inputType = InputType.TYPE_NULL
                f.wmParams.alpha = 1.0f
                f.wmParams.flags = WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE or
                        WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL or
                        WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN or
                        WindowManager.LayoutParams.FLAG_LAYOUT_INSET_DECOR or
                        WindowManager.LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH
                f.updateParams()
                ADIManager.startForDefaultEvents(activity, sample.toFloat())
                started = true
            }

        }

        f.setLayout(view)
        f.show()
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