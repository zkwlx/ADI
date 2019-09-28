package com.adi.ui

import android.annotation.TargetApi
import android.app.Activity
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import android.provider.Settings
import android.text.InputType
import android.text.TextUtils
import android.view.LayoutInflater
import android.view.View
import android.view.WindowManager
import android.widget.*
import com.adi.ADIConfig
import com.adi.ADIManager
import com.adi.R


/**
 * @author zhoukewen
 * @since 2019-09-12
 */
object ADIFloatManager {

    @TargetApi(Build.VERSION_CODES.M)
    fun showADIFloat(activity: Activity) {
        if (!Settings.canDrawOverlays(activity)) {
            val intent = Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION)
            activity.startActivity(intent)
        } else {
            val floatWindow = FloatWindow(activity)
            val view = LayoutInflater.from(activity).inflate(R.layout.adi_float, null)
            val sampleEdit = view.findViewById<EditText>(R.id.sample_edit)
            val adiBtn = view.findViewById<Button>(R.id.adi_btn)
            val adiCloseBtn = view.findViewById<Button>(R.id.adi_close)
            val progress = view.findViewById<ProgressBar>(R.id.progress_bar)
            val defaultInputType = sampleEdit.inputType
            val threadContendCheck = view.findViewById<RadioButton>(R.id.thread_contend_check)
            val objectAllocCheck = view.findViewById<RadioButton>(R.id.object_alloc_check)
            val adiEnableGroup = view.findViewById<RadioGroup>(R.id.adi_enable_group)
            val depthEdit = view.findViewById<EditText>(R.id.depth_edit)
            var started = false

            fun updateForStarted() {
                progress.visibility = View.VISIBLE
                adiBtn.setBackgroundResource(android.R.drawable.ic_media_pause)
                sampleEdit.inputType = InputType.TYPE_NULL
                threadContendCheck.isClickable = false
                objectAllocCheck.isClickable = false
                floatWindow.wmParams.alpha = 1.0f
                floatWindow.wmParams.flags = WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE or
                        WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL or
                        WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN or
                        WindowManager.LayoutParams.FLAG_LAYOUT_INSET_DECOR or
                        WindowManager.LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH
                floatWindow.updateParams()
            }

            fun updateForStop() {
                progress.visibility = View.INVISIBLE
                adiBtn.setBackgroundResource(android.R.drawable.ic_media_play)
                sampleEdit.inputType = defaultInputType
                threadContendCheck.isClickable = true
                objectAllocCheck.isClickable = true
                floatWindow.wmParams.alpha = 0.6f
                floatWindow.wmParams.flags = WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL or
                        WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN or
                        WindowManager.LayoutParams.FLAG_LAYOUT_INSET_DECOR or
                        WindowManager.LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH
                floatWindow.updateParams()
            }

            adiCloseBtn.setOnClickListener {
                if (started) {
                    updateForStop()
                    ADIManager.stop()
                    started = false
                }
                floatWindow.close()
            }

            adiBtn.setOnClickListener {
                if (started) {
                    updateForStop()
                    ADIManager.stop()
                    started = false
                } else {
                    updateForStarted()

                    ADIManager.init(activity)

                    val type = when (adiEnableGroup.checkedRadioButtonId) {
                        R.id.object_alloc_check -> ADIConfig.Type.OBJECT_ALLOC_AND_FREE
                        R.id.thread_contend_check -> ADIConfig.Type.THREAD_MONITOR_CONTEND
                        else -> null
                    }
                    if (type != null) {
                        val builder = ADIConfig.Builder()
                        builder.setEventType(type)
                        val sample = sampleEdit.text.toString()
                        if (!TextUtils.isEmpty(sample)) {
                            builder.setSampleInterval(sample.toFloat())
                        }
                        val depth = depthEdit.text.toString()
                        if (!TextUtils.isEmpty(depth)) {
                            builder.setStackDepth(depth.toInt())
                        }
                        ADIManager.start(activity, builder.build())
                        started = true
                    }
                }
            }
            floatWindow.setLayout(view)
            floatWindow.show()
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