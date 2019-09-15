package com.adi.ui

import android.annotation.SuppressLint
import android.content.Context
import android.content.Context.WINDOW_SERVICE
import android.graphics.PixelFormat
import android.os.Build
import android.view.*
import kotlin.math.abs
import kotlin.math.roundToInt


/**
 * @author zhoukewen
 * @since 2019-09-12
 */
@SuppressLint("ClickableViewAccessibility")
class FloatWindow(
    private val mContext: Context // 声明一个上下文对象
) : View(mContext) {
    private val wm: WindowManager =
        mContext.getSystemService(WINDOW_SERVICE) as WindowManager // 声明一个窗口管理器对象
    private var mContentView: View? = null // 声明一个内容视图对象
    private var mScreenX: Float = 0.toFloat()
    private var mScreenY: Float = 0.toFloat() // 触摸点在屏幕上的横纵坐标
    private var mLastX: Float = 0.toFloat()
    private var mLastY: Float = 0.toFloat() // 上次触摸点的横纵坐标
    private var mDownX: Float = 0.toFloat()
    private var mDownY: Float = 0.toFloat() // 按下点的横纵坐标
    // 判断悬浮窗是否打开
    private var isShow = false

    private var mListener: FloatClickListener? = null // 声明一个悬浮窗的点击监听器对象

    init {
        // 从系统服务中获取窗口管理器，后续将通过该管理器添加悬浮窗
        if (wmParams == null) {
            wmParams = WindowManager.LayoutParams()
        }
    }

    // 设置悬浮窗的内容布局
    fun setLayout(layoutId: Int) {
        // 从指定资源编号的布局文件中获取内容视图对象
        mContentView = LayoutInflater.from(mContext).inflate(layoutId, null)
        setLayout(mContentView)
    }

    fun setLayout(view: View?) {
        mContentView = view
        // 接管悬浮窗的触摸事件，使之即可随手势拖动，又可处理点击动作
        mContentView!!.setOnTouchListener { v, event ->
            // 在发生触摸事件时触发
            mScreenX = event.rawX
            mScreenY = event.rawY
            when (event.action) {
                MotionEvent.ACTION_DOWN -> {
                    mDownX = mScreenX
                    mDownY = mScreenY
                }
                MotionEvent.ACTION_MOVE -> updateViewPosition() // 更新视图的位置
                MotionEvent.ACTION_UP -> {
                    updateViewPosition() // 更新视图的位置
                    // 响应悬浮窗的点击事件
                    if (abs(mScreenX - mDownX) < 3 && abs(mScreenY - mDownY) < 3) {
                        if (mListener != null) {
                            mListener!!.onFloatClick(v)
                        }
                    }
                }
            }
            mLastX = mScreenX
            mLastY = mScreenY
            true
        }
    }

    // 更新悬浮窗的视图位置
    private fun updateViewPosition() {
        // 此处不能直接转为整型，因为小数部分会被截掉，重复多次后就会造成偏移越来越大
        wmParams!!.x = (wmParams!!.x + mScreenX - mLastX).roundToInt()
        wmParams!!.y = (wmParams!!.y + mScreenY - mLastY).roundToInt()
        // 通过窗口管理器更新内容视图的布局参数
        wm.updateViewLayout(mContentView, wmParams)
    }

    // 显示悬浮窗
    fun show() {
        if (mContentView != null) {
            // 设置为TYPE_SYSTEM_ALERT类型，才能悬浮在其它页面之上
            if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
                // 注意TYPE_SYSTEM_ALERT从Android8.0开始被舍弃了
                wmParams!!.type = WindowManager.LayoutParams.TYPE_SYSTEM_ALERT
            } else {
                // 从Android8.0开始悬浮窗要使用TYPE_APPLICATION_OVERLAY
                wmParams!!.type = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY
            }
            wmParams!!.format = PixelFormat.RGBA_8888
            wmParams!!.flags = WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
            wmParams!!.alpha = 1.0f // 1.0为完全不透明，0.0为完全透明
            // 对齐方式为靠左且靠上，因此悬浮窗的初始位置在屏幕的左上角
            wmParams!!.gravity = Gravity.CENTER
            wmParams!!.x = 0
            wmParams!!.y = 0
            // 设置悬浮窗的宽度和高度为自适应
            wmParams!!.width = WindowManager.LayoutParams.WRAP_CONTENT
            wmParams!!.height = WindowManager.LayoutParams.WRAP_CONTENT
            // 添加自定义的窗口布局，然后屏幕上就能看到悬浮窗了
            wm.addView(mContentView, wmParams)
            isShow = true
        }
    }

    // 关闭悬浮窗
    fun close() {
        if (mContentView != null) {
            // 移除自定义的窗口布局
            wm.removeView(mContentView)
            isShow = false
        }
    }

    // 设置悬浮窗的点击监听器
    fun setOnFloatListener(listener: FloatClickListener) {
        mListener = listener
    }

    // 定义一个悬浮窗的点击监听器接口，用于触发点击行为
    interface FloatClickListener {
        fun onFloatClick(v: View)
    }

    companion object {
        private val TAG = "FloatWindow"
        private var wmParams: WindowManager.LayoutParams? = null
    }

}
