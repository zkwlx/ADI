//
// Created by kewen on 2019-08-30.
//

#ifndef ADI_JDI_NATIVE_H
#define ADI_JDI_NATIVE_H

#include <jni.h>
#include "../jvmti.h"


/**
 * 获取指定对象的大小。对象的大小与JVM的具体实现相关，
 * 是该对象所占用存储空间的近似值，可能会包含某些或所有对象的开销，
 * 因此对象大小的比较，只在某个JVM实现内有意义，在不同JVM实现之间没有比较意思。
 * 对象的大小，在单次调用期间，也可能会发生变化。
 */
extern "C" JNIEXPORT jlong JNICALL getObjectSize(JNIEnv *env, jclass jclazz, jobject obj);

jvmtiEnv *getJvmtiEnv(JavaVM *vm);

jvmtiEnv *getJvmtiEnvFromJNI(JNIEnv *env);

JavaVM *getJavaVM(JNIEnv *env);

#endif //ADI_JDI_NATIVE_H
