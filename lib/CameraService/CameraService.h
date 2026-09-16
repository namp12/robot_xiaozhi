#pragma once
#include <Arduino.h>

class CameraService {
public:
    static bool init();
    static bool startTask(uint8_t core_id = 0);
    static void requestCapture();
};
