//
// Created by kewen on 2019-08-30.
//

#ifndef ADI_THREADSTARTHANDLER_H
#define ADI_THREADSTARTHANDLER_H

#include <jni.h>
#include "../jvmti.h"

void JNICALL ThreadStart(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread);

#endif //ADI_THREADSTARTHANDLER_H
