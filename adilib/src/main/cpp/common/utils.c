//
// Created by kewen on 2019-08-28.
//

#include "utils.h"

#include <sys/time.h>
#include "log.h"

int64_t currentTimeMillis() {
    struct timeval tv = {};
    gettimeofday(&tv, NULL);
    int64_t xFactor = 1;
    int64_t result = (int64_t) ((xFactor * tv.tv_sec * 1000) + (tv.tv_usec / 1000));
    return result;
}