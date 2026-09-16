#pragma once
#include <Arduino.h>

class MotorDriver {
public:
    static bool init();
    static void forward(int speed = 255);
    static void backward(int speed = 255);
    static void turnLeft(int speed = 200);
    static void turnRight(int speed = 200);
    static void stop();
};
