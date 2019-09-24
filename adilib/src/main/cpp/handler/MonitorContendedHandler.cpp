//
// Created by kewen on 2019-09-17.
//

#include <cstdio>
#include <pthread.h>
#include "MonitorContendedHandler.h"
#include "../common/log.h"
#include "../common/jdi_native.h"
#include "Config.h"


extern "C" {
#include "../dumper.h"
#include "../common/utils.h"
}
#define LOG_TAG "MCE"

/**
 * 创建 MCE 事件的基础信息
 * 格式：
 * 时间戳^^^竞争线程名^^^锁对象 Hash^^^锁对象名字
 * 示例：
 * 1569222849005^^^Binder:5199_4^^^75353688^^^Landroid/os/MessageQueue;
 *
 * @param jvmti_env
 * @param jni_env
 * @param thread
 * @param object
 * @return
 */
static char *
createMCEBaseInfo(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jobject object) {
    jint hashcode;
    jvmti_env->GetObjectHashCode(object, &hashcode);
    jclass klass = jni_env->GetObjectClass(object);
    char *signature;
    jvmti_env->GetClassSignature(klass, &signature, nullptr);
    jvmtiThreadInfo threadInfo;
    jvmti_env->GetThreadInfo(thread, &threadInfo);
    char *baseInfo;
    int64_t timeMillis = currentTimeMillis();
    asprintf(&baseInfo, "%lld%s%s%s%d%s%s",
             timeMillis, SEP_POWER,
             threadInfo.name, SEP_POWER,
             hashcode, SEP_POWER,
             signature);
    jvmti_env->Deallocate((unsigned char *) signature);
    return baseInfo;
}

/**
 * 创建 MCED 事件的基础信息
 * 格式：
 * 时间戳^^^竞争线程名^^^锁对象 Hash
 * 示例：
 * 569222849006^^^Binder:5199_4^^^75353688
 *
 * @param jvmti_env
 * @param jni_env
 * @param thread
 * @param object
 * @return
 */
static char *
createMCEDBaseInfo(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jobject object) {
    jint hashcode;
    jvmti_env->GetObjectHashCode(object, &hashcode);
    jvmtiThreadInfo threadInfo;
    jvmti_env->GetThreadInfo(thread, &threadInfo);
    char *baseInfo;
    int64_t timeMillis = currentTimeMillis();
    asprintf(&baseInfo, "%lld%s%s%s%d",
             timeMillis, SEP_POWER,
             threadInfo.name, SEP_POWER,
             hashcode);
    return baseInfo;
}

/**
 * 创建锁信息
 * 格式：
 * 持有锁的线程名^^^该线程持有次数^^^等待这个锁的线程数量^^^等待这个锁通知的线程数量
 * 示例：
 * monitor_test_thread2^^^1^^^0^^^0
 * @param jvmti_env
 * @param jni_env
 * @param thread
 * @param monitorUsage
 * @return
 */
static char *
createMonitorInfo(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread,
                  jvmtiMonitorUsage monitorUsage) {
    jvmtiThreadInfo ownerThreadInfo;
    jvmti_env->GetThreadInfo(monitorUsage.owner, &ownerThreadInfo);
    char *monitorInfo;
    asprintf(&monitorInfo, "%s%s%d%s%d%s%d",
             ownerThreadInfo.name, SEP_POWER,
             monitorUsage.entry_count, SEP_POWER,
             monitorUsage.waiter_count, SEP_POWER,
             monitorUsage.notify_waiter_count);
    return monitorInfo;
}

static int stackDepth = 0;

void JNICALL
MonitorContendedEnter(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jobject object) {
    ALOGI("======MonitorContendedEnter=======");
    char *baseInfo = createMCEBaseInfo(jvmti_env, jni_env, thread, object);
    jvmtiMonitorUsage monitorUsage;
    jvmti_env->GetObjectMonitorUsage(object, &monitorUsage);
    char *monitorInfo = createMonitorInfo(jvmti_env, jni_env, thread, monitorUsage);
    if (stackDepth == 0) {
        stackDepth = getStackDepth();
    }
    // 获取竞争线程的调用栈
    char *contendStackInfo = createStackInfo(jvmti_env, jni_env, thread, stackDepth);
    // 获取持有锁线程的调用栈
    char *ownerStackInfo = createStackInfo(jvmti_env, jni_env, monitorUsage.owner, stackDepth);
    char *line;
    asprintf(&line, "%s|%s|%s|%s|%s\n", LOG_TAG, baseInfo, monitorInfo, contendStackInfo,
             ownerStackInfo);
    dumper_add(line);
}

void JNICALL
MonitorContendedEntered(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jobject object) {
    ALOGI("======MonitorContendedEntered=======");
    char *baseInfo = createMCEDBaseInfo(jvmti_env, jni_env, thread, object);
    char *line;
    asprintf(&line, "%sD|%s\n", LOG_TAG, baseInfo);
    dumper_add(line);
}

