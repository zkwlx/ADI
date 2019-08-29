//
// Created by kewen on 2019-08-28.
//

#include <android/log.h>
#include <string.h>
#include "looper_test.h"
#include "looper.h"

#define LOG_TAG "looper_test"
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static message_looper_t *looper;

static void _handle(message_t *msg) {
    ALOGI(" -----> %s", (char *) msg->data);
}

void test_looper_start() {
    looper = looperCreate(_handle);
    if (looper == NULL) {
        ALOGI("looperCreate fail!!!!!!!!!");
        return;
    }

    if (looperStart(looper) == LOOPER_START_THREAD_ERROR) {
        ALOGI("looperStart thread create fail!!!!!!!!!");
        looperDestroy(&looper);
        return;
    }
}

void test_looper_destroy() {
    looperDestroy(&looper);
}

void test_looper_push(char *data) {
    looperPost(looper, 0, data, strlen(data) + 1);
}