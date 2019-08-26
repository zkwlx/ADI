//
// Created by kewen on 2019-08-21.
//

#ifndef ADI_METHODENTRYHANDLER_H
#define ADI_METHODENTRYHANDLER_H

#include <jni.h>
#include "../jvmti.h"

void MethodEntry(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jmethodID method);

#endif //ADI_METHODENTRYHANDLER_H
