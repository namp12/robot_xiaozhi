#pragma once
#include <Arduino.h>

class AudioService {
public:
    static bool init();
    static void playSound(const char* sound_name);
    static void startListening();
    static void stopListening();
};
