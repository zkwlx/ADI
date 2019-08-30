//
// Created by kewen on 2019-08-30.
//

#include <jni.h>
#include "jdi_native.h"
#include "../jvmti.h"
#include "../common/log.h"

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