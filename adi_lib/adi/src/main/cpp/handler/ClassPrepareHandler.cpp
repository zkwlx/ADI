//
// Created by kewen on 2019-10-31.
//

#include <cstring>
#include "ClassPrepareHandler.h"

#include "../common/log.h"
#include "../common/jdi_native.h"
#include "Terminator.h"

void JNICALL ClassPrepare(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jclass klass) {
    if (isTerminated()) {
        ALOGI(">>>classPrepare: terminated!");
        return;
    }
    char *classSignature;
    jvmti_env->GetClassSignature(klass, &classSignature, nullptr);

    char *stackInfo = createStackInfo(jvmti_env, jni_env, thread, 10);
    ALOGI("----> classPrepare: %s, %s", classSignature, stackInfo);

    jvmti_env->Deallocate((unsigned char *) classSignature);

}