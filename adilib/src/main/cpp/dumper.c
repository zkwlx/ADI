//
// Created by kewen on 2019-08-15.
//

#include "dumper.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "clooper/looper.h"
#include <errno.h>
#include <malloc.h>
#include "common/log.h"

#define FLUSH_THRESHOLD 10
#define LOG_TAG "Dumper"

static message_looper_t *looper;

static FILE *dumpFile = NULL;

static int writeCount = 0;

void _dump_to_file(message_t *msg) {
    ALOGI("%s", (char *) msg->data);
    fwrite(msg->data, sizeof(char), (msg->data_size - 1), dumpFile);
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
    looper = looperCreate(_dump_to_file);
    if (looper == NULL) {
        ALOGI("looperCreate fail!!!!!!!!!");
        return;
    }
    if (looperStart(looper) == 0) {
        create_file(dumpDir);
    } else {
        switch (looperStart(looper)) {
            case LOOPER_START_THREAD_ERROR:
                ALOGI("looperStart thread create fail.");
                looperDestroy(&looper);
                break;
            case LOOPER_START_REPEAT_ERROR:
                ALOGI("looperStart looper is started");
                break;
            case LOOPER_IS_NULL:
                ALOGI("looperStart looper is NULL");
                break;
            default:
                break;
        }
    }
}

void dumper_stop() {
    if (dumpFile != NULL) {
        fflush(dumpFile);
        if (fclose(dumpFile)) {
            ALOGI("%s", "文件关闭成功");
        }
        dumpFile = NULL;
    }
    looperDestroy(&looper);
}


void dumper_add(char *data) {
    looperPost(looper, 0, data, strlen(data) + 1);
    free(data);
}