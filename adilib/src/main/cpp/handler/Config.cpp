//
// Created by kewen on 2019-09-06.
//

#include "Config.h"

static float sampleIntervalUs;
static int stackDepth;

void setSampleIntervalMs(float ms) {
    sampleIntervalUs = ms * 1000;
}

float getSampleIntervalUs() {
    return sampleIntervalUs;
}

void setStackDepth(int depth) {
    stackDepth = depth;
}

int getStackDepth() {
    return stackDepth;
}

