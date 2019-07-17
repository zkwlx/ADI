package jdi.client

import com.sun.jdi.Bootstrap
import com.sun.jdi.Location
import com.sun.jdi.StringReference
import com.sun.jdi.VirtualMachine
import com.sun.jdi.event.*
import com.sun.jdi.request.EventRequest
import com.sun.jdi.request.EventRequestManager


class SimpleDebugger {

    private lateinit var vm: VirtualMachine
    private lateinit var process: Process
    private lateinit var eventRequestManager: EventRequestManager
    private lateinit var eventQueue: EventQueue
    private lateinit var eventSet: EventSet
    private var vmExit = false

    fun init() {
        val launchingConnector = Bootstrap.virtualMachineManager().defaultConnector()

        // Get arguments of the launching connector
        val defaultArguments = launchingConnector.defaultArguments()
        val mainArg = defaultArguments["main"]
        val suspendArg = defaultArguments["suspend"]
        // Set class of main method
        mainArg!!.setValue("jdi.demo.Hello")
        suspendArg!!.setValue("true")
        vm = launchingConnector.launch(defaultArguments)

        process = vm.process()

        // Register ClassPrepareRequest
        eventRequestManager = vm.eventRequestManager()
        val classPrepareRequest = eventRequestManager.createClassPrepareRequest()
        classPrepareRequest.addClassFilter("jdi.demo.Hello")
        classPrepareRequest.addCountFilter(1)
        classPrepareRequest.setSuspendPolicy(EventRequest.SUSPEND_ALL)
        classPrepareRequest.enable()

        // Enter event loop
        eventLoop()

        process.destroy()
    }

    @Throws(Exception::class)
    private fun eventLoop() {
        eventQueue = vm.eventQueue()
        while (true) {
            if (vmExit) {
                break
            }
            eventSet = eventQueue.remove()
            val eventIterator = eventSet.eventIterator()
            while (eventIterator.hasNext()) {
                val event = eventIterator.next() as Event
                execute(event)
            }
        }
    }

    @Throws(Exception::class)
    private fun execute(event: Event) {
        if (event is VMStartEvent) {
            println("VM started")
            eventSet.resume()
        } else if (event is ClassPrepareEvent) {
            val mainClassName = event.referenceType().name()
            if (mainClassName == "jdi.demo.Hello") {
                println("Class $mainClassName is already prepared")
            }
            // Get location
            val referenceType = event.referenceType()
            val locations = referenceType.locationsOfLine(10)
            val location = locations.get(0) as Location

            // Create BreakpointEvent
            val breakpointRequest = eventRequestManager
                .createBreakpointRequest(location)
            breakpointRequest.setSuspendPolicy(EventRequest.SUSPEND_ALL)
            breakpointRequest.enable()
            eventSet.resume()
        } else if (event is BreakpointEvent) {
            println("Reach line 10 of jdi.demo.Hello")
            val threadReference = event.thread()
            val stackFrame = threadReference.frame(0)
            val localVariable = stackFrame
                .visibleVariableByName("str")
            val value = stackFrame.getValue(localVariable)
            val str = (value as StringReference).value()
            println(
                ("The local variable str at line 10 is $str of ${value.type().name()}")
            )
            eventSet.resume()
        } else if (event is VMDisconnectEvent) {
            vmExit = true
        } else {
            eventSet.resume()
        }
    }

}

fun main() {
    val debugger = SimpleDebugger()
    debugger.init()

}
