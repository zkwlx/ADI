//
// Created by kewen on 2019-08-19.
//

#ifndef ADI_VMOBJECTALLOCHANDLER_H
#define ADI_VMOBJECTALLOCHANDLER_H

#include <jni.h>
#include "../jvmti.h"

void ObjectAllocCallback(jvmtiEnv *jvmti, JNIEnv *env, jthread thread, jobject object, jclass klass,
                         jlong size);

void setVMObjectAllocSampleInterval(float intervalMs);

#endif //ADI_VMOBJECTALLOCHANDLER_H
