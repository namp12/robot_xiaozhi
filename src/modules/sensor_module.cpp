#include "modules/sensor_module.h"

#if ENABLE_MODULE_SENSOR
#include "event_bus.h"
#include <esp_log.h>

static const char *TAG = "SENSOR_MODULE";

bool sensor_module_init() {
    pinMode(PIN_TOUCH_SENSOR, INPUT);
    ESP_LOGI(TAG, "Khoi tao Sensor Module (Touch chan %d, VL53L0X I2C)", PIN_TOUCH_SENSOR);
    return true;
}

int sensor_get_distance_cm() {
    // Khi ban lap VL53L0X, them doc khoang cach tai day
    return 100;
}

bool sensor_is_touched() {
    return (digitalRead(PIN_TOUCH_SENSOR) == HIGH);
}

#endif // ENABLE_MODULE_SENSOR
