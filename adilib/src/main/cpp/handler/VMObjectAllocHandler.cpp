//
// Created by kewen on 2019-08-19.
//

#include "VMObjectAllocHandler.h"

extern "C" {
#include "../dumper.h"
#include "../common/utils.h"
}

#include <jni.h>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include "../common/log.h"


#define LOG_TAG "OA"

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
        //获取方法签名
        err = jvmti->GetMethodName(info.method, &methodName, &methodSignature, nullptr);
        if (err != JVMTI_ERROR_NONE) {
            ALOGE("[JVMTI ERROR on GetMethodName]:%i", err);
            break;
        }
        //获取方法所在类
        jclass declaringClass;
        err = jvmti->GetMethodDeclaringClass(info.method, &declaringClass);
        if (err != JVMTI_ERROR_NONE) {
            ALOGE("[JVMTI ERROR on GetMethodDeclaringClass]:%i", err);
            break;
        }
        //获取方法所在类的签名
        err = jvmti->GetClassSignature(declaringClass, &classSignature, nullptr);
        if (err != JVMTI_ERROR_NONE) {
            ALOGE("[JVMTI ERROR on GetClassSignature]:%i", err);
            break;
        }
//        ALOGI("-----------------method: %s %s", classSignature, methodName);

        if (result == nullptr) {
            asprintf(&result, "%s%s%s%s%s", classSignature, SEP_POWER, methodName, SEP_POWER,
                     methodSignature);
        } else {
            char *stack;
            asprintf(&stack, "%s%s%s%s%s%s%s",
                     result, SEP_COMMA,
                     classSignature, SEP_POWER,
                     methodName, SEP_POWER,
                     methodSignature);
            free(result);
            result = stack;
        }
        jvmti->Deallocate((unsigned char *) classSignature);
        jvmti->Deallocate((unsigned char *) methodName);
        jvmti->Deallocate((unsigned char *) methodSignature);
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
    long timeMillis = currentTimeMillis();
    // TODO 白名单逻辑
//    char *findSelf = strstr(classSignature, "com/adi/");
//    char *findAndroid = strstr(classSignature, "android");
//    if (findSelf == nullptr && findAndroid == nullptr) {
//        return nullptr;
//    }
    asprintf(&baseInfo, "%ld%s%s%s%s%s%lli",
             timeMillis, SEP_POWER,
             threadInfo.name, SEP_POWER,
             classSignature, SEP_POWER,
             size);
    ALOGI("[base:] %s", baseInfo);
    jvmti->Deallocate((unsigned char *) classSignature);
    return baseInfo;
}

/**
 * 生成的日志格式：
 * <p>
 * 事件名称|时间戳 线程名 对象签名 对象大小|调用栈第一行(方法所在类签名 方法名 方法签名),调用栈第二行,调用栈第三行
 *
 */
void ObjectAllocCallback(jvmtiEnv *jvmti, JNIEnv *env, jthread thread, jobject object, jclass klass,
                         jlong size) {
    ALOGI("==========alloc callback==========");

    char *baseInfo = createBaseInfo(jvmti, env, thread, object, klass, size);
    if (baseInfo == nullptr) {
        return;
    }
    char *stackInfo = createStackInfo(jvmti, env, thread);
    char *line;
    asprintf(&line, "%s|%s|%s\n", LOG_TAG, baseInfo, stackInfo);
    free(baseInfo);
    free(stackInfo);
    ALOGI("%s", line);
    dumper_add(line);
}
