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


#define LOG_TAG "OA"

static float sampleInterval = 0;
static int startTime = 0;

static char *
createBaseInfo(jvmtiEnv *jvmti, JNIEnv *env, jthread thread, jobject object, jclass klass,
               jlong size) {
    jvmtiThreadInfo threadInfo;
    jvmti->GetThreadInfo(thread, &threadInfo);
    char *classSignature;
    jvmti->GetClassSignature(klass, &classSignature, nullptr);
    char *baseInfo;
    long timeMillis = currentTimeMillis();
    // TODO 白名单逻辑
//    char *findSelf = strstr(classSignature, "com/adi/");
//    char *findAndroid = strstr(classSignature, "android");
//    if (findSelf == nullptr && findAndroid == nullptr) {
//        return nullptr;
//    }
    asprintf(&baseInfo, "%ld%s%s%s%s%s%lli",
             timeMillis, SEP_POWER,
             threadInfo.name, SEP_POWER,
             classSignature, SEP_POWER,
             size);
    ALOGI("[base:] %s", baseInfo);
    jvmti->Deallocate((unsigned char *) classSignature);
    return baseInfo;
}

/**
 * 生成的日志格式：
 * <p>
 * 事件名称|时间戳 线程名 对象签名 对象大小|调用栈第一行(方法所在类签名 方法名 方法签名),调用栈第二行,调用栈第三行
 *
 */
void ObjectAllocCallback(jvmtiEnv *jvmti, JNIEnv *env, jthread thread, jobject object, jclass klass,
                         jlong size) {
    timeval tv{};
    gettimeofday(&tv, nullptr);
    int now = tv.tv_usec;
    if (now - startTime <= sampleInterval) {
        ALOGD("=======Object Alloc too fast!=======");
        return;
    }
    startTime = now;

    ALOGI("==========Object Alloc dump~ =========");
    char *baseInfo = createBaseInfo(jvmti, env, thread, object, klass, size);
    if (baseInfo == nullptr) {
        return;
    }
    char *stackInfo = createStackInfo(jvmti, env, thread, 10);
    char *line;
    asprintf(&line, "%s|%s|%s\n", LOG_TAG, baseInfo, stackInfo);
    free(baseInfo);
    free(stackInfo);
    ALOGI("%s", line);
    dumper_add(line);
}

void setVMObjectAllocSampleInterval(float intervalMs) {
    sampleInterval = intervalMs * 1000;
}