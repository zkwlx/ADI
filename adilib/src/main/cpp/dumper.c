//
// Created by kewen on 2019-08-15.
//

#include "dumper.h"
#include <unistd.h>
#include <android/log.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "clooper/looper.h"
#include <errno.h>
#include <malloc.h>

#define FLUSH_THRESHOLD 10
#define LOG_TAG "adi"
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static message_looper_t *looper;

static FILE *dumpFile = NULL;

static int writeCount = 0;

void _dump_to_file(message_t *msg) {
//    ALOGI("%s", (char *) msg->data);
    fwrite(msg->data, sizeof(char), (size_t) msg->data_size, dumpFile);
    if (++writeCount > FLUSH_THRESHOLD) {
        ALOGI("%s", "........ flush to file ...........");
        fflush(dumpFile);
        writeCount = 0;
    }
}

static void create_file(char *dumpDir) {
    if (dumpFile == NULL) {
        struct timeval stamp;
        gettimeofday(&stamp, NULL);
        char *filePath;
        asprintf(&filePath, "%s/adi_%ld.log", dumpDir, stamp.tv_sec);
        dumpFile = fopen(filePath, "ae");
        if (dumpFile == NULL) {
            ALOGE("文件打开错误：%s，错误原因：%s", filePath, strerror(errno));
        }
    }
}

void dumper_start(char *dumpDir) {
    ALOGI("Dumper 文件目录：%s", dumpDir);
    if (create_looper(&looper, _dump_to_file)) {
        ALOGI("create_looper fail.");
        return;
    }
    if (start_loop(looper)) {
        ALOGI("start_loop fail.");
        destroy_looper(looper);
        return;
    }
    create_file(dumpDir);
}

void dumper_stop() {
    if (dumpFile != NULL) {
        fflush(dumpFile);
        if (fclose(dumpFile)) {
            ALOGI("%s", "文件关闭成功");
        }
    }
    destroy_looper(looper);
}


void dumper_add(char *data) {
    push_message(looper, 0, data, (int) strlen(data));
    free(data);
}