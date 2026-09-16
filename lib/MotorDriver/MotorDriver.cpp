#include "MotorDriver.h"
#include "app_config.h"
#include <esp_log.h>

static const char *TAG = "MOTOR_DRIVER";

bool MotorDriver::init() {
#if ENABLE_MODULE_MOTOR
    pinMode(PIN_MOTOR_LEFT_IN1, OUTPUT);
    pinMode(PIN_MOTOR_LEFT_IN2, OUTPUT);
    pinMode(PIN_MOTOR_RIGHT_IN3, OUTPUT);
    pinMode(PIN_MOTOR_RIGHT_IN4, OUTPUT);
    stop();
    ESP_LOGI(TAG, "Khoi tao MotorDriver thanh cong!");
    return true;
#else
    return false;
#endif
}

void MotorDriver::forward(int speed) {
#if ENABLE_MODULE_MOTOR
    digitalWrite(PIN_MOTOR_LEFT_IN1, HIGH);
    digitalWrite(PIN_MOTOR_LEFT_IN2, LOW);
    digitalWrite(PIN_MOTOR_RIGHT_IN3, HIGH);
    digitalWrite(PIN_MOTOR_RIGHT_IN4, LOW);
#endif
}

void MotorDriver::backward(int speed) {
#if ENABLE_MODULE_MOTOR
    digitalWrite(PIN_MOTOR_LEFT_IN1, LOW);
    digitalWrite(PIN_MOTOR_LEFT_IN2, HIGH);
    digitalWrite(PIN_MOTOR_RIGHT_IN3, LOW);
    digitalWrite(PIN_MOTOR_RIGHT_IN4, HIGH);
#endif
}

void MotorDriver::turnLeft(int speed) {
#if ENABLE_MODULE_MOTOR
    digitalWrite(PIN_MOTOR_LEFT_IN1, LOW);
    digitalWrite(PIN_MOTOR_LEFT_IN2, HIGH);
    digitalWrite(PIN_MOTOR_RIGHT_IN3, HIGH);
    digitalWrite(PIN_MOTOR_RIGHT_IN4, LOW);
#endif
}

void MotorDriver::turnRight(int speed) {
#if ENABLE_MODULE_MOTOR
    digitalWrite(PIN_MOTOR_LEFT_IN1, HIGH);
    digitalWrite(PIN_MOTOR_LEFT_IN2, LOW);
    digitalWrite(PIN_MOTOR_RIGHT_IN3, LOW);
    digitalWrite(PIN_MOTOR_RIGHT_IN4, HIGH);
#endif
}

void MotorDriver::stop() {
#if ENABLE_MODULE_MOTOR
    digitalWrite(PIN_MOTOR_LEFT_IN1, LOW);
    digitalWrite(PIN_MOTOR_LEFT_IN2, LOW);
    digitalWrite(PIN_MOTOR_RIGHT_IN3, LOW);
    digitalWrite(PIN_MOTOR_RIGHT_IN4, LOW);
#endif
}
