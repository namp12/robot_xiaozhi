#pragma once
#include <Arduino.h>
#include "app_config.h"

#if ENABLE_MODULE_MOTOR

bool motor_module_init();
void motor_forward(int speed = 255);
void motor_backward(int speed = 255);
void motor_turn_left(int speed = 200);
void motor_turn_right(int speed = 200);
void motor_stop();

#endif // ENABLE_MODULE_MOTOR
