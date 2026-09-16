#pragma once
#include <Arduino.h>
#include "app_config.h"

enum RobotEmotion {
    EMOTION_NORMAL,
    EMOTION_HAPPY,
    EMOTION_SAD,
    EMOTION_SURPRISED,
    EMOTION_ANGRY
};

#if ENABLE_MODULE_DISPLAY

bool display_module_init();
void display_set_emotion(RobotEmotion emotion);

#endif // ENABLE_MODULE_DISPLAY
