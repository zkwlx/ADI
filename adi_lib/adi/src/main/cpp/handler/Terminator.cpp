//
// Created by kewen on 2019-11-05.
//

#include "Terminator.h"

static volatile bool terminated = false;

void setTerminated(bool isTerminated) {
    terminated = isTerminated;
}

bool isTerminated() {
    return terminated;
}
