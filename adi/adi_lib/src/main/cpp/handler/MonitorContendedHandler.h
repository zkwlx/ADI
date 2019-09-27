//
// Created by kewen on 2019-09-17.
//

#ifndef ADI_MONITORCONTENDEDHANDLER_H
#define ADI_MONITORCONTENDEDHANDLER_H

#include <jni.h>
#include "../jvmti.h"

void JNICALL
MonitorContendedEnter(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jobject object);

void JNICALL
MonitorContendedEntered(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jobject object);

#endif //ADI_MONITORCONTENDEDHANDLER_H
