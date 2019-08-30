//
// Created by kewen on 2019-08-30.
//

#include <cstdio>
#include "GCCallbackHandler.h"
#include "../common/log.h"

extern "C" {
#include "../dumper.h"
#include "../common/utils.h"
}

#define LOG_TAG "GC"

static char *createBaseInfo(jvmtiEnv *jvmti_env) {
    char *baseInfo;
    long timeMillis = currentTimeMillis();
    asprintf(&baseInfo, "%ld", timeMillis);
    return baseInfo;
}

void GCStartCallback(jvmtiEnv *jvmti) {
    char *baseInfo = createBaseInfo(jvmti);
    char *line;
    asprintf(&line, "%sS|%s\n", LOG_TAG, baseInfo);
    dumper_add(line);
}

void GCFinishCallback(jvmtiEnv *jvmti) {
    char *baseInfo = createBaseInfo(jvmti);
    char *line;
    asprintf(&line, "%sF|%s\n", LOG_TAG, baseInfo);
    dumper_add(line);
}