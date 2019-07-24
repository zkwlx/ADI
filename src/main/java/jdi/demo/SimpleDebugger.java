package jdi.demo;

import com.sun.jdi.*;
import com.sun.jdi.connect.Connector;
import com.sun.jdi.connect.IllegalConnectorArgumentsException;
import com.sun.jdi.connect.LaunchingConnector;
import com.sun.jdi.connect.VMStartException;
import com.sun.jdi.event.*;
import com.sun.jdi.request.BreakpointRequest;
import com.sun.jdi.request.ClassPrepareRequest;
import com.sun.jdi.request.EventRequest;
import com.sun.jdi.request.EventRequestManager;

import java.io.IOException;
import java.util.List;
import java.util.Map;


class SimpleDebugger {

    private VirtualMachine vm;
    private EventRequestManager eventRequestManager;
    private EventSet eventSet;
    private boolean vmExit = false;

    void launch() {

        log("start for debugger");
        vm = launchTarget("jdi.demo.Hello");

        Process process = vm.process();

        // Register ClassPrepareRequest
        eventRequestManager = vm.eventRequestManager();
        ClassPrepareRequest classPrepareRequest
                = eventRequestManager.createClassPrepareRequest();
        classPrepareRequest.addClassFilter("jdi.demo.Hello");
        classPrepareRequest.addCountFilter(1);
        classPrepareRequest.setSuspendPolicy(EventRequest.SUSPEND_ALL);
        classPrepareRequest.enable();

        // Enter event loop
        try {
            eventLoop();
        } catch (Exception e) {
            e.printStackTrace();
        }

        process.destroy();
    }

    /**
     * 参数：
     * mainArgs为目标程序main函数所在的类
     **/
    private VirtualMachine launchTarget(String mainArgs) {
        //findLaunchingConnector：获取连接
        LaunchingConnector connector = findLaunchingConnector();
        //connectorArguments：设置连接参数
        Map arguments = connectorArguments(connector, mainArgs);
        try {
            return connector.launch(arguments);//启动连接
        } catch (IOException exc) {
            throw new Error("Unable to launch target VM: " + exc);
        } catch (IllegalConnectorArgumentsException exc) {
            throw new Error("Internal error: " + exc);
        } catch (VMStartException exc) {
            throw new Error("Target VM failed to initialize: " + exc.getMessage());
        }
    }

    private LaunchingConnector findLaunchingConnector() {
        VirtualMachineManager vmm = Bootstrap.virtualMachineManager();
        return vmm.defaultConnector();
    }

    /**
     * 参数：
     * connector为清单1.中获取的Connector连接实例
     * mainArgs为目标程序main函数所在的类
     **/
    private Map connectorArguments(LaunchingConnector connector, String mainArgs) {
        Map arguments = connector.defaultArguments();
        Connector.Argument mainArg = (Connector.Argument) arguments.get("main");
        if (mainArg == null) {
            throw new Error("Bad launching connector");
        }
        mainArg.setValue(mainArgs);
        return arguments;
    }

    private void eventLoop() throws Exception {
        EventQueue eventQueue = vm.eventQueue();
        while (!vmExit) {
            eventSet = eventQueue.remove();
            EventIterator eventIterator = eventSet.eventIterator();
            while (eventIterator.hasNext()) {
                Event event = eventIterator.next();
                execute(event);
            }
        }
    }

    private void execute(Event event) throws Exception {
        if (event instanceof VMStartEvent) {
            log("----> VMStartEvent");
            eventSet.resume();
        } else if (event instanceof ClassPrepareEvent) {
            log("----> ClassPrepareEvent");
            ClassPrepareEvent classPrepareEvent = (ClassPrepareEvent) event;
            String mainClassName = classPrepareEvent.referenceType().name();
            if (mainClassName.equals("jdi.demo.Hello")) {
                log("Class " + mainClassName + " is already prepared");
            }
            // Get location
            ReferenceType referenceType = classPrepareEvent.referenceType();
            List locations = referenceType.locationsOfLine(10);
            Location location = (Location) locations.get(0);

            // Create BreakpointEvent
            BreakpointRequest breakpointRequest = eventRequestManager
                    .createBreakpointRequest(location);
            breakpointRequest.setSuspendPolicy(EventRequest.SUSPEND_ALL);
            breakpointRequest.enable();
            eventSet.resume();
        } else if (event instanceof BreakpointEvent) {
            log("----> BreakpointEvent");
            log("Reach line 10 of jdi.demo.Hello");
            BreakpointEvent breakpointEvent = (BreakpointEvent) event;
            ThreadReference threadReference = breakpointEvent.thread();
            StackFrame stackFrame = threadReference.frame(0);
            LocalVariable localVariable = stackFrame
                    .visibleVariableByName("str");
            Value value = stackFrame.getValue(localVariable);
            String str = ((StringReference) value).value();
            log("The local variable str at line 10 is " + str + " of " + value.type().name());
            eventSet.resume();
        } else if (event instanceof VMDisconnectEvent) {
            vmExit = true;
            log("----> VMDisconnectEvent");
        } else {
            log("----> " + event.toString());
            eventSet.resume();
        }
    }

    private static void log(String content) {
        System.out.println(content);
    }

}
