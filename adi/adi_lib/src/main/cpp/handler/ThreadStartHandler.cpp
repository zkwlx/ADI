//
// Created by kewen on 2019-08-30.
//

#include <cstdio>
#include "ThreadStartHandler.h"
#include "../common/log.h"

extern "C" {
#include "../dumper.h"
#include "../common/utils.h"
}

#define LOG_TAG "TS"


static char *createBaseInfo(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread) {
    jvmtiThreadInfo threadInfo;
    jvmti_env->GetThreadInfo(thread, &threadInfo);
    char *baseInfo;
    int64_t timeMillis = currentTimeMillis();
    asprintf(&baseInfo, "%lld%s%s", timeMillis, SEP_POWER, threadInfo.name);
    return baseInfo;
}

void JNICALL ThreadStart(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread) {
    char *baseInfo = createBaseInfo(jvmti_env, jni_env, thread);
    char *line;
    asprintf(&line, "%s|%s\n", LOG_TAG, baseInfo);
    dumper_add(line);
}
