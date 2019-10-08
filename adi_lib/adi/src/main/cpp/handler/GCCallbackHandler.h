//
// Created by kewen on 2019-08-30.
//

#ifndef ADI_GCCALLBACKHANDLER_H
#define ADI_GCCALLBACKHANDLER_H

#include "../jvmti.h"

void GCStartCallback(jvmtiEnv *jvmti);

void GCFinishCallback(jvmtiEnv *jvmti);

#endif //ADI_GCCALLBACKHANDLER_H
