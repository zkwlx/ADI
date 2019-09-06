//
// Created by kewen on 2019-08-30.
//

#include <jni.h>
#include <cstdlib>
#include "jdi_native.h"
#include "../jvmti.h"
#include "../common/log.h"
#include "utils.h"

/**
 * 使用前要 load so 并且启动 jvmti
 * <p>
 * 获取指定对象的大小。对象的大小与JVM的具体实现相关，
 * 是该对象所占用存储空间的近似值，可能会包含某些或所有对象的开销，
 * 因此对象大小的比较，只在某个JVM实现内有意义，在不同JVM实现之间没有比较意思。
 * 对象的大小，在单次调用期间，也可能会发生变化。
 */
extern "C" JNIEXPORT jlong JNICALL getObjectSize(JNIEnv *env, jclass jclazz, jobject obj) {
    JavaVM *vm = getJavaVM(env);
    jvmtiEnv *jvmtiEnv = getJvmtiEnv(vm);
    jlong size;
    jvmtiEnv->GetObjectSize(obj, &size);
    return size;
}

jvmtiEnv *getJvmtiEnvFromJNI(JNIEnv *env) {
    JavaVM *vm = getJavaVM(env);
    return getJvmtiEnv(vm);
}

jvmtiEnv *getJvmtiEnv(JavaVM *vm) {
    jvmtiEnv *jvmti_env;
    jint result = vm->GetEnv((void **) &jvmti_env, JVMTI_VERSION_1_2);
    if (result != JNI_OK) {
        return nullptr;
    }
    return jvmti_env;
}

JavaVM *getJavaVM(JNIEnv *env) {
    JavaVM *javaVm;
    jint result = env->GetJavaVM(&javaVm);
    if (result != JNI_OK) {
        return nullptr;
    }
    return javaVm;
}

char *createStackInfo(jvmtiEnv *jvmti, JNIEnv *env, jthread thread, int stackDepth) {
    char *result = nullptr;
    jvmtiFrameInfo frames[stackDepth];
    jint count;
    jvmtiError err;

    err = jvmti->GetStackTrace(thread, 0, stackDepth, frames, &count);
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

