#include "SensorService.h"
#include "app_config.h"
#include <esp_log.h>

static const char *TAG = "SENSOR_SERVICE";

bool SensorService::init() {
#if ENABLE_MODULE_SENSOR
    pinMode(PIN_TOUCH_SENSOR, INPUT);
    ESP_LOGI(TAG, "Khoi tao SensorService (Touch pin %d)", PIN_TOUCH_SENSOR);
    return true;
#else
    return false;
#endif
}

int SensorService::getDistanceCm() {
#if ENABLE_MODULE_SENSOR
    return 100;
#else
    return -1;
#endif
}

bool SensorService::isTouched() {
#if ENABLE_MODULE_SENSOR
    return (digitalRead(PIN_TOUCH_SENSOR) == HIGH);
#else
    return false;
#endif
}
