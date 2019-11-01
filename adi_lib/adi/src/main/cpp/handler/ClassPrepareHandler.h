//
// Created by kewen on 2019-10-31.
//

#ifndef ADI_LIB_CLASSPREPAREHANDLER_H
#define ADI_LIB_CLASSPREPAREHANDLER_H

#include <jni.h>
#include "../jvmti.h"

void JNICALL ClassPrepare(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread, jclass klass);

#endif //ADI_LIB_CLASSPREPAREHANDLER_H
