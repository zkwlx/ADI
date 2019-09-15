package com.adi.ui

import android.content.Context
import android.content.Context.WINDOW_SERVICE
import android.util.AttributeSet
import android.util.DisplayMetrics
import android.view.LayoutInflater
import android.view.MotionEvent
import android.view.View
import android.view.WindowManager
import android.widget.Button
import android.widget.FrameLayout
import com.adi.R
import kotlin.math.abs


class FloatLayout(context: Context, attrs: AttributeSet?) : FrameLayout(context, attrs) {
    private val mWindowManager: WindowManager =
        context.getSystemService(WINDOW_SERVICE) as WindowManager
    private val mFloatView: Button
    private var startTime: Long = 0
    private var mTouchStartX: Float = 0.toFloat()
    private var mTouchStartY: Float = 0.toFloat()
    private var isclick: Boolean = false
    private var mWmParams: WindowManager.LayoutParams? = null
    private var endTime: Long = 0
    private var mOnFloatViewClick: OnFloatViewClick? = null

    init {
        LayoutInflater.from(context).inflate(R.layout.adi_float, this)
        //浮动窗口按钮
        mFloatView = findViewById<View>(R.id.adi_start) as Button
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        // 获取相对屏幕的坐标，即以屏幕左上角为原点
        val x = event.rawX.toInt()
        val y = event.rawY.toInt()
        //下面的这些事件，跟图标的移动无关，为了区分开拖动和点击事件
        when (event.action) {
            MotionEvent.ACTION_DOWN -> {
                startTime = System.currentTimeMillis()
                mTouchStartX = event.x
                mTouchStartY = event.y
            }
            MotionEvent.ACTION_MOVE -> {
                //图标移动的逻辑在这里
                val mMoveStartX = event.x
                val mMoveStartY = event.y
                // 如果移动量大于3才移动
                if (abs(mTouchStartX - mMoveStartX) > 3 && abs(mTouchStartY - mMoveStartY) > 3) {
                    // 更新浮动窗口位置参数
                    mWmParams!!.x = (x - mTouchStartX).toInt()
                    mWmParams!!.y = (y - mTouchStartY).toInt()
                    mWindowManager.updateViewLayout(this, mWmParams)
                    return false
                }
            }
            MotionEvent.ACTION_UP -> {
                endTime = System.currentTimeMillis()
                val endX = event.rawX
                val dm = DisplayMetrics()
                mWindowManager.defaultDisplay.getMetrics(dm)
                val screenWidth = dm.widthPixels
                //根据最终手指停留位置，悬浮窗吸边效果实现
                if (endX > screenWidth / 2) {
                    mWmParams!!.x = screenWidth - width
                    mWmParams!!.y = (y - mTouchStartY).toInt()
                    mWindowManager.updateViewLayout(this, mWmParams)
                } else {
                    mWmParams!!.x = 0
                    mWmParams!!.y = (y - mTouchStartY).toInt()
                    mWindowManager.updateViewLayout(this, mWmParams)
                }

                val cha = endTime - startTime
                val upX = event.x
                val upY = event.y
                var isYi = true
                //当从点击到弹起小于半秒的时候,则判断为点击,如果超过则不响应点击事件
                if (abs(upX - mTouchStartX) < 30 && abs(upY - mTouchStartY) < 30) {
                    isYi = false
                }

                isclick = if (cha > 0.3 * 1000L) {
                    false
                } else {
                    !isYi
                }
            }
        }
        //响应点击事件
        if (isclick) {
            if (mOnFloatViewClick != null) {
                mOnFloatViewClick!!.onClick()
                isclick = false
            }
        }
        return true
    }

    fun setOnFloatViewClick(onFloatViewClick: OnFloatViewClick) {
        mOnFloatViewClick = onFloatViewClick
    }

    interface OnFloatViewClick {
        fun onClick()
    }

    /**
     * 将小悬浮窗的参数传入，用于更新小悬浮窗的位置。
     *
     * @param params 小悬浮窗的参数
     */
    fun setParams(params: WindowManager.LayoutParams) {
        mWmParams = params
    }
}