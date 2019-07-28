package jdi.demo;

import com.sun.jdi.connect.IllegalConnectorArgumentsException;

import java.io.IOException;

public class Launcher {

    public static void main(String[] args) {
//        SimpleDebugger debugger = new SimpleDebugger();
//        debugger.launch();
        AttachDebugger debugger = new AttachDebugger();
        try {
            debugger.attach();
        } catch (IllegalAccessException | IllegalConnectorArgumentsException | IOException e) {
            e.printStackTrace();
        }
    }
}