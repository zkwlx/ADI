//
// Created by kewen on 2019-08-28.
//

#include "utils.h"

#include <sys/time.h>

long currentTimeMillis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}