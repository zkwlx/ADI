//
// Created by kewen on 2019-08-15.
//

#include "dumper.h"
#include <unistd.h>
#include <android/log.h>
#include <stdio.h>
#include <string.h>
#include "clooper/looper.h"

#define LOG_TAG "adi"
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

message_looper_t *looper;


void _dump_to_file(message_t *msg) {
    ALOGI("%s", (char *) msg->data);
}

void dumper_start() {
    if (create_looper(&looper, _dump_to_file)) {
        ALOGI("create_looper fail.");
        return;
    }

    if (start_loop(looper)) {
        ALOGI("start_loop fail.");
        destroy_looper(looper);
        return;
    }

    ALOGI("=====> looper start: %p <=======", &looper);
}

void dumper_stop() {
    destroy_looper(looper);
}


void dumper_add(char *data) {
    ALOGI("%i ~~~~~~~~~~~~ %p", (int) strlen(data), &looper);
    push_message(looper, 0, data, (int) strlen(data));
}