package com.adi.demo;

import android.util.Log;

/**
 * @author zhoukewen
 * @since 2019-10-24
 */
public class DemoObject2 {

    public static void publicStaticMethod(String str) {
        Log.i("zkw", "------>>>>>so nice, demo2 !!!!" + str);
    }

    private void privateMethod() {
        Log.i("zkw", "xxx");
    }

    public void publicMethod() {
        Log.i("zkw", "xxx");
    }

    protected void protectedMethod() {
        Log.i("zkw", "xxx");
    }

    public final void publicFinalMethod() {
        Log.i("zkw", "xxx");
    }

    protected final void protectedFinalMethod() {
        Log.i("zkw", "xxx");
    }
}
