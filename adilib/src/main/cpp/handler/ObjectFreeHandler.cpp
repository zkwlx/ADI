//
// Created by kewen on 2019-09-10.
//

#include "ObjectFreeHandler.h"
#include <cstdio>
#include "../common/log.h"

extern "C" {
#include "../dumper.h"
#include "../common/utils.h"
}
#define LOG_TAG "OF"

static char *createBaseInfo(jvmtiEnv *jvmti_env, jlong tag) {
    char *baseInfo;
    int64_t timeMillis = currentTimeMillis();
    asprintf(&baseInfo, "%lld%s%lld",
             timeMillis, SEP_POWER,
             tag);
    return baseInfo;
}

void JNICALL ObjectFree(jvmtiEnv *jvmti_env, jlong tag) {
    char *baseInfo = createBaseInfo(jvmti_env, tag);
    char *line;
    asprintf(&line, "%s|%s\n", LOG_TAG, baseInfo);
    dumper_add(line);
}
