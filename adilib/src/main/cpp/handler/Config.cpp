//
// Created by kewen on 2019-09-06.
//

#include "Config.h"
#include "VMObjectAllocHandler.h"

static int sampleIntervalMs = 0;

void setSampleIntervalMs(int ms) {
    setVMObjectAllocSampleInterval(ms);
}

