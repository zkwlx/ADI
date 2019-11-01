//
// Created by kewen on 2019-10-31.
//

#ifndef ADI_LIB_FRAMEPOPHANDLER_H
#define ADI_LIB_FRAMEPOPHANDLER_H

#include <jni.h>
#include "../jvmti.h"

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
                      jboolean was_popped_by_exception);

#endif //ADI_LIB_FRAMEPOPHANDLER_H
