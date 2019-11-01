//
// Created by kewen on 2019-10-31.
//

#include "FramePopHandler.h"
#include "../common/log.h"
#include "../common/jdi_native.h"

/**
 * Frame 弹出事件。暂未支持，不要使用！
 * @deprecated
 * @param jvmti_env
 * @param jni_env
 * @param thread
 * @param method
 * @param was_popped_by_exception
 */
void JNICALL FramePop(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jmethodID method,
                      jboolean was_popped_by_exception) {
    jvmtiError err;

    jclass declaringClass = nullptr;
    err = jvmti_env->GetMethodDeclaringClass(method, &declaringClass);
    if (err != JVMTI_ERROR_NONE) {
        ALOGE("[JVMTI ERROR on GetMethodDeclaringClass]:%i", err);
    }
    char *classSignature = nullptr;
    err = jvmti_env->GetClassSignature(declaringClass, &classSignature, nullptr);
    if (err != JVMTI_ERROR_NONE) {
        ALOGE("[JVMTI ERROR on GetClassSignature]:%i", err);
    }

    char *methodName = nullptr;
    err = jvmti_env->GetMethodName(method, &methodName, nullptr, nullptr);
    if (err != JVMTI_ERROR_NONE) {
        ALOGE("[JVMTI ERROR on GetMethodName]:%i", err);
    }

    char *stackInfo = createStackInfo(jvmti_env, jni_env, thread, 10);

    ALOGI("----> framePop: %s, %s, %s", classSignature, methodName, stackInfo);
}