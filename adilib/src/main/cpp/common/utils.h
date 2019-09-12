//
// Created by kewen on 2019-08-28.
//

#ifndef ADI_UTILS_H
#define ADI_UTILS_H

#define SEP_POWER "^^^"
#define SEP_COMMA ",,,"

#include <stdint.h>

/**
 * 获取当前毫秒时间戳
 * @return
 */
int64_t currentTimeMillis();

/**
 * 获取当前微秒时间戳
 * 1 ms = 1000 us
 * @return
 */
int64_t currentTimeMicro();

#endif //ADI_UTILS_H
