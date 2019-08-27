//
// Created by kewen on 2019-08-19.
//

#include "VMObjectAllocHandler.h"

extern "C" {
#include "../dumper.h"
}

#include <jni.h>
#include <string>
#include "../jvmti.h"
#include <android/log.h>
#include <sstream>
#include <cstring>
#include <unistd.h>


#define LOG_TAG "ObjectAlloc"

#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

constexpr int FRAME_COUNT = 10;

static char *createStackInfo(jvmtiEnv *jvmti, JNIEnv *env, jthread thread) {
    char *result = nullptr;
    jvmtiFrameInfo frames[FRAME_COUNT];
    jint count;
    jvmtiError err;

    err = jvmti->GetStackTrace(thread, 0, FRAME_COUNT, frames, &count);
    if (err != JVMTI_ERROR_NONE) {
        ALOGE("[JVMTI ERROR on GetStackTrace]: %i", err);
        return result;
    }
    if (count <= 0) {
        return result;
    }
    for (int i = 0; i < count; i++) {
        jvmtiFrameInfo info = frames[i];
        char *classSignature;
        char *methodName;
        char *methodSignature;
        err = jvmti->GetMethodName(info.method, &methodName, &methodSignature, nullptr);
        if (err != JVMTI_ERROR_NONE) {
            ALOGE("[JVMTI ERROR on GetMethodName]:%i", err);
            break;
        }
        jclass declaringClass;
        err = jvmti->GetMethodDeclaringClass(info.method, &declaringClass);
        if (err != JVMTI_ERROR_NONE) {
            ALOGE("[JVMTI ERROR on GetMethodDeclaringClass]:%i", err);
            break;
        }

        err = jvmti->GetClassSignature(declaringClass, &classSignature, nullptr);
        if (err != JVMTI_ERROR_NONE) {
            ALOGE("[JVMTI ERROR on GetClassSignature]:%i", err);
            break;
        }
        ALOGI("-----------------method: %s %s", classSignature, methodName);

        if (result == nullptr) {
            asprintf(&result, "%s %s %s", classSignature, methodName, methodSignature);
        } else {
            char *stack;
            asprintf(&stack, "%s,%s %s %s", result, classSignature, methodName,
                     methodSignature);
            free(result);
            result = stack;
        }
        jvmti->Deallocate((unsigned char *) classSignature);
        jvmti->Deallocate((unsigned char *) methodName);
        jvmti->Deallocate((unsigned char *) methodSignature);
//        ALOGI("####: %s", result);
    }
    return result;
}

static char *
createBaseInfo(jvmtiEnv *jvmti, JNIEnv *env, jthread thread, jobject object, jclass klass,
               jlong size) {
    jvmtiThreadInfo threadInfo;
    jvmti->GetThreadInfo(thread, &threadInfo);
    char *classSignature;
    jvmti->GetClassSignature(klass, &classSignature, nullptr);
    char *baseInfo;
    //TODO 添加时间戳
//    char *findSelf = strstr(classSignature, "com/adi/");
//    char *findAndroid = strstr(classSignature, "android");
//    if (findSelf == nullptr && findAndroid == nullptr) {
//        return nullptr;
//    }
    asprintf(&baseInfo, "%s %s %lli", threadInfo.name, classSignature, size);
    ALOGI("[base:] %s", baseInfo);
    jvmti->Deallocate((unsigned char *) classSignature);
    return baseInfo;
}

void ObjectAllocCallback(jvmtiEnv *jvmti, JNIEnv *env, jthread thread, jobject object, jclass klass,
                         jlong size) {
    ALOGI("==========alloc callback==========");

    char *baseInfo = createBaseInfo(jvmti, env, thread, object, klass, size);
    if (baseInfo == nullptr) {
        return;
    }
    char *stackInfo = createStackInfo(jvmti, env, thread);
//    ALOGI("base: %s | stack: %s", baseInfo, stackInfo);

    char *line;
    asprintf(&line, "OA|%s|%s\n", baseInfo, stackInfo);
    free(baseInfo);
    free(stackInfo);
//    ALOGI("%s", line);
    dumper_add(line);
}
