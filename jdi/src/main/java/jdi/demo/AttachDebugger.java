package jdi.demo;

import com.sun.jdi.*;
import com.sun.jdi.connect.AttachingConnector;
import com.sun.jdi.connect.Connector;
import com.sun.jdi.connect.IllegalConnectorArgumentsException;
import com.sun.jdi.event.*;
import com.sun.jdi.request.BreakpointRequest;
import com.sun.jdi.request.EventRequest;
import com.sun.jdi.request.EventRequestManager;
import com.sun.jdi.request.MethodEntryRequest;
import com.sun.tools.jdi.SocketAttachingConnector;

import java.io.IOException;
import java.util.List;
import java.util.Map;

class AttachDebugger {

    private VirtualMachine vm;
    private EventSet eventSet;
    private boolean vmExit = false;

    void attach() throws IllegalAccessException, IllegalConnectorArgumentsException, IOException {

        vm = attachTarget();

        List<ReferenceType> classesByName = vm.classesByName("jdi.demo.Hello");
        if (classesByName == null || classesByName.size() == 0) {
            System.out.println("No class found");
            return;
        }
        ReferenceType classRefType = classesByName.get(0);
        List<Method> methodsByName = classRefType.methodsByName("printHello");
        if (methodsByName == null || methodsByName.size() == 0) {
            System.out.println("No method found");
            return;
        }
        Method method = methodsByName.get(0);

        vm.setDebugTraceMode(VirtualMachine.TRACE_ALL);
        vm.resume();
        EventRequestManager erm = vm.eventRequestManager();

        MethodEntryRequest methodEntryRequest = erm.createMethodEntryRequest();
        methodEntryRequest.addClassFilter(classRefType);
        methodEntryRequest.setSuspendPolicy(EventRequest.SUSPEND_NONE);
        methodEntryRequest.enable();

        BreakpointRequest breakpointRequest = erm
                .createBreakpointRequest(method.location());
        breakpointRequest.setSuspendPolicy(EventRequest.SUSPEND_EVENT_THREAD);
        breakpointRequest.enable();

        try {
            eventLoop();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private VirtualMachine attachTarget() throws IllegalAccessException, IOException, IllegalConnectorArgumentsException {
        SocketAttachingConnector connector = findSocketAttachingConnector();
        Map arguments = connector.defaultArguments();
        Connector.Argument hostArg = (Connector.Argument) arguments.get("hostname");// hostname
        Connector.Argument portArg = (Connector.Argument) arguments.get("port");// port

        hostArg.setValue("127.0.0.1");
        portArg.setValue(String.valueOf(8800));

        return connector.attach(arguments);
    }

    private SocketAttachingConnector findSocketAttachingConnector() throws IllegalAccessException {
        VirtualMachineManager vmm = Bootstrap.virtualMachineManager();
        List<AttachingConnector> connectors = vmm.attachingConnectors();
        SocketAttachingConnector sac = null;
        for (AttachingConnector ac : connectors) {
            if (ac instanceof SocketAttachingConnector) {
                sac = (SocketAttachingConnector) ac;
                break;
            }
        }
        if (sac == null) {
            System.out.println("JDI error");
            throw new IllegalAccessException("SocketAttachingConnector is null!!");
        } else {
            return sac;
        }
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

    private void execute(Event event) {
        if (event instanceof VMStartEvent) {
            System.out.println("VM started");
            eventSet.resume();
        } else if (event instanceof BreakpointEvent) {
            System.out.println("Reach Method printHello of test.Test");
            eventSet.resume();
        } else if (event instanceof MethodEntryEvent) {
            MethodEntryEvent mee = (MethodEntryEvent) event;
            Method method = mee.method();
            System.out.println(method.name() + " was Entered!");
            eventSet.resume();
        } else if (event instanceof VMDisconnectEvent) {
            vmExit = true;
        } else {
            eventSet.resume();
        }
    }

}
