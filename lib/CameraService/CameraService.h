#pragma once
#include <Arduino.h>
#include <esp_camera.h>

class CameraService {
public:
    static bool init();
    static bool startTask(uint8_t core_id = 0);
    static void requestCapture();
    static camera_fb_t* getFrame(uint32_t timeout_ms = 100);
    static void returnFrame(camera_fb_t *fb);
    static bool isReady();
};
