#pragma once
#include <Arduino.h>

class SystemBrain {
public:
    static bool init(uint8_t core_id = 1);
};
