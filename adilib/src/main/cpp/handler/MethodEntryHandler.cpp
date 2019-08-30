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
    char *result = nullptr;
    jvmtiError err;
//    char *classSignature = nullptr;
    char *methodName = nullptr;
//    char *methodSignature = nullptr;
    err = jvmti->GetMethodName(method, &methodName, nullptr, nullptr);
    if (err != JVMTI_ERROR_NONE) {
        ALOGE("[JVMTI ERROR on GetMethodName]:%i", err);
        return result;
    }
//    jclass declaringClass = nullptr;
//    err = jvmti->GetMethodDeclaringClass(method, &declaringClass);
//    if (err != JVMTI_ERROR_NONE) {
//        ALOGE("[JVMTI ERROR on GetMethodDeclaringClass]:%i", err);
//        return result;
//    }

//            jclass cls = env->FindClass("java/lang/Class");
//            jmethodID mid_getName = env->GetMethodID(cls, "getName", "()Ljava/lang/String;");
//            jstring name = static_cast<jstring>(env->CallObjectMethod(declaringClass, mid_getName));
//            const char *className = env->GetStringUTFChars(name, JNI_FALSE);
//            ALOGI("-----------------%s", className);
//            if (strcmp(className, "com.android.internal.os.ZygoteInit") == 0) {
//                break;
//            }
//            env->ReleaseStringUTFChars(name, className);

//    err = jvmti->GetClassSignature(declaringClass, &classSignature, nullptr);
//    if (err != JVMTI_ERROR_NONE) {
//        ALOGE("[JVMTI ERROR on GetClassSignature]:%i", err);
//        return result;
//    }

//    asprintf(&result, "%s", methodName);
    ALOGI("++++: %s", methodName);
//    jvmti->Deallocate((unsigned char *) classSignature);
    jvmti->Deallocate((unsigned char *) methodName);
//    jvmti->Deallocate((unsigned char *) methodSignature);
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