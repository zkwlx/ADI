# ADI Introduction
<a href='https://bintray.com/zkwlx/ADI/zkw.adi/0.9.3/link'>
  <img src='https://api.bintray.com/packages/zkwlx/ADI/zkw.adi/images/download.svg?version=0.9.3'>
</a>
<a href='https://github.com/zkwlx/ADI/blob/master/LICENSE'>
  <img src='https://img.shields.io/github/license/zkwlx/ADI'>
</a>

(中文版本请点击 [这里](https://github.com/zkwlx/ADI/wiki/ADI-%E7%AE%80%E4%BB%8B) )

<p>
<table cellspacing="10">
<tr>

  <td>
  <a href="https://zkwlx.github.io/ADI/docs/adi_对象分配.html">
  <img alt="Example of object allocation and release" src="docs/对象分配与释放.png"  />
    <p/>Object allocation, Click for try
  </a>

  </td>

  <td>
  <a href="https://zkwlx.github.io/ADI/docs/adi_线程竞争.html">
  <img alt="Example of multithreaded contended" src="docs/多线程竞争.png" />
    <p/>Multithreaded contended, Click for try
  </a>
  </td>

  <td>
  <a href="https://github.com/zkwlx/ADI/blob/master/docs/adi_screenshot.png">
  <img alt="ADI Controller" src="docs/adi_screenshot.png" />
    <p/>ADI Controller
  </a>
  </td>

</tr>
</table>
</p>

ADI（Android Debug Intensive）is an enhanced tool set for implementing Android application development debugging through JVMTI.Currently, it mainly provides performance-related monitoring capabilities.
ADI collects performance data from running APP and provides tools for generating analysis charts.  
The following features are currently available:
* Monitors the allocation and release of objects during ART running, including object size, creating object call stacks, and more.
* Monitors the monitor contend between multiple threads during the ART running, including the call stack when the contention occurs, the waiting time of the current contend thread, and the thread currently holding the lock.
> Note that ADI only supports systems after Android 8.0 (including 8.0).
# How to use
### Integrated into APP
First add jcenter dependencies (added please ignore):
```gradle
buildscript {
    repositories {
        jcenter()
    }
}
allprojects {
    repositories {
        jcenter()
    }
}
```
then APP works with ADI:
```gradle
debugImplementation 'zkw.adi:adi:0.9.3'
releaseImplementation 'zkw.adi:adi-nop:0.9.3'
```
### Generate a Log file
After you integrate the ADI library, you can perform performance data collection on the APP.  
There are two ways to collect.
##### Method 1: Use the start/stop interface
The specific code is as follows:
```kotlin
// Initialize first.
ADIManager.init(context)
...
// Add the following code wherever you want to start collecting.
val builder = ADIConfig.Builder()
builder.setEventType(ADIConfig.Type.THREAD_MONITOR_CONTEND)// Set the type of event to monitor.
ADIManager.start(activity, builder.build())
...
// Add the following code wherever you want to stop collecting.
ADIManager.stop()
```
##### Method 2: Interactive floating control window
The control window can change the ADI configuration without recompiling.  
The following code shows how to start the control window:
```kotlin
// called in the Activity
ADIFloatManager.showADIFloat(activity)
```
Please refer to the diagram for how to use the control window:

<img width="300" height="232" alt="Control window" src="docs/adi_float.png" />

### Parse the Log file and generate a chart
The collected content is stored in the `Context.getExternalCacheDir()`/ADI directory, for example:
```bash
/sdcard/Android/data/APP Package Name/cache/ADI/adi_1570605092.log
```
After removing the Log file, you need to analyze the Log with `adi_analyzer` tool and generate a chart, which is installed via pip3:
```bash
pip3 install adi-analyzer
```
Then run adi_analyzer and pass the log file:
```bash
adi-analyzer ~/adi_1570605092.log
```
adi_anaylzer will create a chart file ending in .html, and then automatically launch the browser to open the chart file. Please refer to the cover for the effect of the chart.

# Detailed function
## Object allocation monitoring
When monitoring object allocation, ADI will monitor the creation events of Java layer objects. There are several points to note:
* Frequently creating objects can cause APPs to get stuck or even get ANR. The sampling interval can be configured via [ADIConfig.sampleIntervalMs](https://github.com/zkwlx/ADI/blob/master/adi_lib/adi/src/main/java/com/adi/ADIConfig.java#L68) or control window (default 0.8 ms)
* The depth of the call stack defaults to 10. Can be modified by [ADIConfig.stackDepth](https://github.com/zkwlx/ADI/blob/master/adi_lib/adi/src/main/java/com/adi/ADIConfig.java#L68) or control window.
* If the generated Log file is too large to affect the chart generation time, it is recommended to increase the sampling interval or reduce the overall sampling duration.

ADI generates two charts for the Log of the object event: the object allocation quantity chart and the object allocation size chart. Let's use the Object allocation chart to describe how to use the chart below.
<a href="https://zkwlx.github.io/ADI/docs/adi_对象分配.html">
 <img alt="chart to describe" src="docs/对象分配图表图解.png" />
 <p/>Click for try
</a>

## Multi-threaded competition monitoring
When monitoring multi-threaded contention, ADI monitors multi-threaded lock contention events caused by the `synchronized` keywords of all Java layers.  
The following points need to be noted:
* The depth of the call stack defaults to 10 and can be modified via [ADIConfig.stackDepth](https://github.com/zkwlx/ADI/blob/master/adi_lib/adi/src/main/java/com/adi/ADIConfig.java#L68) or the control window.
* If the generated Log file is too large to affect the chart generation time, it is recommended to reduce the sampling duration.

The Y-axis of the multi-threaded competition chart is the name of the thread in which the competition occurs, and the X-axis is the time. For details, see the diagram.
<a href="https://zkwlx.github.io/ADI/docs/adi_线程竞争.html">
 <img alt="chart to describe" src="docs/线程竞争图表图解.png" />
 <p/>Click for try
</a>

# License
```
Copyright 2019 Square, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
