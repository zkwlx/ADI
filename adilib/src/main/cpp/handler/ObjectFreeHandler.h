//
// Created by kewen on 2019-09-10.
//

#ifndef ADI_OBJECTFREEHANDLER_H
#define ADI_OBJECTFREEHANDLER_H

#include "../jvmti.h"

void JNICALL ObjectFree(jvmtiEnv *jvmti_env, jlong tag);

#endif //ADI_OBJECTFREEHANDLER_H
