//
// Created by kewen on 2019-08-28.
//

#include "utils.h"

#include <sys/time.h>
#include "log.h"

/**
 * 获取当前毫秒时间戳
 * @return
 */
int64_t currentTimeMillis() {
    struct timeval tv = {};
    gettimeofday(&tv, NULL);
    int64_t factor = 1000;
    int64_t result = (int64_t) ((factor * tv.tv_sec) + (tv.tv_usec / 1000));
    return result;
}

/**
 * 获取当前微秒时间戳
 * 1 ms = 1000 us
 * @return
 */
int64_t currentTimeMicro() {
    struct timeval tv = {};
    gettimeofday(&tv, NULL);
    int64_t factor = 1000000;
    int64_t result = (int64_t) ((factor * tv.tv_sec) + tv.tv_usec);
    return result;
}