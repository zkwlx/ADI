package com.dodola.jvmti;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.dodola.jvmtilib.JVMTIHelper;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button tv = findViewById(R.id.sample_text);
        tv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                JVMTIHelper.init(MainActivity.this);
            }
        });
        findViewById(R.id.button_gc).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                System.gc();
                System.runFinalization();
            }
        });
        findViewById(R.id.button_modify_class).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                JVMTIHelper.retransformClasses(new Class[]{Activity.class});
            }
        });
        findViewById(R.id.button_start_activity).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, Main2Activity.class));
            }
        });
    }

}

