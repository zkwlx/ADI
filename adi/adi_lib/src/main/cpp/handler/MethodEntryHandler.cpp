//
// Created by kewen on 2019-08-21.
//

#include "MethodEntryHandler.h"

extern "C" {
#include "../dumper.h"
}

#include <jni.h>
#include <string>
#include "../jvmti.h"
#include <sstream>
#include <cstring>
#include <unistd.h>
#include "../common/log.h"

#define LOG_TAG "MethodEntry"

static char *createMethodInfo(jvmtiEnv *jvmti, JNIEnv *env, jthread thread, jmethodID method) {
    return const_cast<char *>("");
}

static char *createBaseInfo(jvmtiEnv *jvmti, jthread thread) {
    jvmtiThreadInfo threadInfo;
    jvmti->GetThreadInfo(thread, &threadInfo);
    char *baseInfo;
    //TODO 添加时间戳

    asprintf(&baseInfo, "%s", threadInfo.name);
    ALOGI("[base:] %s", baseInfo);
    return baseInfo;
}

void MethodEntry(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jmethodID method) {
    ALOGI("==========method entry callback==========");

    char *baseInfo = createBaseInfo(jvmti_env, thread);
    char *methodInfo = createMethodInfo(jvmti_env, jni_env, thread, method);

    char *line;
    asprintf(&line, "ME|%s|%s\n", baseInfo, methodInfo);
    free(baseInfo);
    free(methodInfo);
    dumper_add(line);
}