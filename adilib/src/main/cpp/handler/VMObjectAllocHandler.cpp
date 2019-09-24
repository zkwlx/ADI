//
// Created by kewen on 2019-08-19.
//

#include "VMObjectAllocHandler.h"


extern "C" {
#include "../dumper.h"
#include "../common/utils.h"
}

#include <jni.h>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include "../common/log.h"
#include "../common/jdi_native.h"
#include "Config.h"


#define LOG_TAG "OA"

static float sampleInterval = 0;
static int stackDepth = 0;
static int64_t startTime = 0;

static char *
createBaseInfo(jvmtiEnv *jvmti, JNIEnv *env, jthread thread, jobject object, jclass klass,
               jlong size, jlong tag) {
    jvmtiThreadInfo threadInfo;
    jvmti->GetThreadInfo(thread, &threadInfo);
    char *classSignature;
    jvmti->GetClassSignature(klass, &classSignature, nullptr);
    char *baseInfo;
    int64_t timeMillis = currentTimeMillis();
    asprintf(&baseInfo, "%lld%s%s%s%s%s%lld%s%lld",
             timeMillis, SEP_POWER,
             threadInfo.name, SEP_POWER,
             classSignature, SEP_POWER,
             size, SEP_POWER,
             tag);
    ALOGI("[base:] %s", baseInfo);
    jvmti->Deallocate((unsigned char *) classSignature);
    return baseInfo;
}

static jlong tag = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static jlong createObjectTag() {
    pthread_mutex_lock(&mtx);
    tag += 1;
    pthread_mutex_unlock(&mtx);
    return tag;
}

/**
 * 生成的日志格式：
 * <p>
 * 事件名称|时间戳 线程名 对象签名 对象大小|调用栈第一行(方法所在类签名 方法名 方法签名),调用栈第二行,调用栈第三行
 *
 */
void ObjectAllocCallback(jvmtiEnv *jvmti, JNIEnv *env, jthread thread, jobject object, jclass klass,
                         jlong size) {
    int64_t now = currentTimeMicro();
    if (sampleInterval == 0) {
        sampleInterval = getSampleIntervalUs();
    }
    if (now - startTime <= getSampleIntervalUs()) {
        ALOGI("----Object Alloc too fast!----");
        return;
    }
    startTime = now;

    jlong tag = createObjectTag();
    jvmti->SetTag(object, tag);
    ALOGI("==========Object Alloc dump~ tag: %lld =========", tag);

    char *baseInfo = createBaseInfo(jvmti, env, thread, object, klass, size, tag);

    if (stackDepth == 0) {
        stackDepth = getStackDepth();
    }
    char *stackInfo = createStackInfo(jvmti, env, thread, stackDepth);
    char *line;
    asprintf(&line, "%s|%s|%s\n", LOG_TAG, baseInfo, stackInfo);
    free(baseInfo);
    free(stackInfo);
    ALOGI("%s", line);
    dumper_add(line);

}