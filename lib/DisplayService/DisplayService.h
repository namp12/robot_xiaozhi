#pragma once
#include <Arduino.h>

enum RobotEmotion {
    EMOTION_NORMAL,
    EMOTION_HAPPY,
    EMOTION_SAD,
    EMOTION_SURPRISED,
    EMOTION_ANGRY
};

class DisplayService {
public:
    static bool init();
    static void setEmotion(RobotEmotion emotion);
};
