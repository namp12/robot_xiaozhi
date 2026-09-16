#include "modules/motor_module.h"

#if ENABLE_MODULE_MOTOR
#include <esp_log.h>

static const char *TAG = "MOTOR_MODULE";

bool motor_module_init() {
    pinMode(PIN_MOTOR_LEFT_IN1, OUTPUT);
    pinMode(PIN_MOTOR_LEFT_IN2, OUTPUT);
    pinMode(PIN_MOTOR_RIGHT_IN3, OUTPUT);
    pinMode(PIN_MOTOR_RIGHT_IN4, OUTPUT);
    motor_stop();
    ESP_LOGI(TAG, "Khoi tao Motor Module hoan tat!");
    return true;
}

void motor_forward(int speed) {
    digitalWrite(PIN_MOTOR_LEFT_IN1, HIGH);
    digitalWrite(PIN_MOTOR_LEFT_IN2, LOW);
    digitalWrite(PIN_MOTOR_RIGHT_IN3, HIGH);
    digitalWrite(PIN_MOTOR_RIGHT_IN4, LOW);
}

void motor_backward(int speed) {
    digitalWrite(PIN_MOTOR_LEFT_IN1, LOW);
    digitalWrite(PIN_MOTOR_LEFT_IN2, HIGH);
    digitalWrite(PIN_MOTOR_RIGHT_IN3, LOW);
    digitalWrite(PIN_MOTOR_RIGHT_IN4, HIGH);
}

void motor_turn_left(int speed) {
    digitalWrite(PIN_MOTOR_LEFT_IN1, LOW);
    digitalWrite(PIN_MOTOR_LEFT_IN2, HIGH);
    digitalWrite(PIN_MOTOR_RIGHT_IN3, HIGH);
    digitalWrite(PIN_MOTOR_RIGHT_IN4, LOW);
}

void motor_turn_right(int speed) {
    digitalWrite(PIN_MOTOR_LEFT_IN1, HIGH);
    digitalWrite(PIN_MOTOR_LEFT_IN2, LOW);
    digitalWrite(PIN_MOTOR_RIGHT_IN3, LOW);
    digitalWrite(PIN_MOTOR_RIGHT_IN4, HIGH);
}

void motor_stop() {
    digitalWrite(PIN_MOTOR_LEFT_IN1, LOW);
    digitalWrite(PIN_MOTOR_LEFT_IN2, LOW);
    digitalWrite(PIN_MOTOR_RIGHT_IN3, LOW);
    digitalWrite(PIN_MOTOR_RIGHT_IN4, LOW);
}

#endif // ENABLE_MODULE_MOTOR
