#pragma once
#include <Arduino.h>

class SensorService {
public:
    static bool init();
    static int  getDistanceCm();
    static bool isTouched();
};
