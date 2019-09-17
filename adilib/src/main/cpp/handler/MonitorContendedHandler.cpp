//
// Created by kewen on 2019-09-17.
//

#include <cstdio>
#include <pthread.h>
#include "MonitorContendedHandler.h"
#include "../common/log.h"
#include "../common/jdi_native.h"


extern "C" {
#include "../dumper.h"
#include "../common/utils.h"
}
#define LOG_TAG "MCE"

static char *
createBaseInfo(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jint objHashcode) {
    jvmtiThreadInfo threadInfo;
    jvmti_env->GetThreadInfo(thread, &threadInfo);
    char *baseInfo;
    int64_t timeMillis = currentTimeMillis();
    asprintf(&baseInfo, "%lld%s%s%s%d",
             timeMillis, SEP_POWER,
             threadInfo.name, SEP_POWER,
             objHashcode);
    return baseInfo;
}

static char *
createMonitorInfo(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jobject object) {
    jvmtiMonitorUsage monitorUsage;
    jvmti_env->GetObjectMonitorUsage(object, &monitorUsage);
    jvmtiThreadInfo ownerThreadInfo;
    jvmti_env->GetThreadInfo(monitorUsage.owner, &ownerThreadInfo);
    char *ownerStackInfo = createStackInfo(jvmti_env, jni_env, monitorUsage.owner, 10);
    char *monitorInfo;
    asprintf(&monitorInfo, "%s%s%d%s%d%s%d%s%s",
             ownerThreadInfo.name, SEP_POWER,
             monitorUsage.entry_count, SEP_POWER,
             monitorUsage.waiter_count, SEP_POWER,
             monitorUsage.notify_waiter_count, SEP_POWER,
             ownerStackInfo);
    return monitorInfo;
}

static jlong tag = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static jlong createObjectTag() {
    pthread_mutex_lock(&mtx);
    tag += 1;
    pthread_mutex_unlock(&mtx);
    return tag;
}


void JNICALL
MonitorContendedEnter(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jobject object) {
    jint hashcode;
    jvmti_env->GetObjectHashCode(object, &hashcode);
    char *baseInfo = createBaseInfo(jvmti_env, jni_env, thread, hashcode);
    char *stackInfo = createStackInfo(jvmti_env, jni_env, thread, 10);
    char *monitorInfo = createMonitorInfo(jvmti_env, jni_env, thread, object);
    char *line;
    asprintf(&line, "%s|%s|%s|%s\n", LOG_TAG, baseInfo, stackInfo, monitorInfo);
    dumper_add(line);
}

void JNICALL
MonitorContendedEntered(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jobject object) {
    jint hashcode;
    jvmti_env->GetObjectHashCode(object, &hashcode);
    char *baseInfo = createBaseInfo(jvmti_env, jni_env, thread, hashcode);
    char *line;
    asprintf(&line, "%sD|%s\n", LOG_TAG, baseInfo);
    dumper_add(line);
}

