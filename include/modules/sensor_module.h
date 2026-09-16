#pragma once
#include <Arduino.h>
#include "app_config.h"

#if ENABLE_MODULE_SENSOR

bool sensor_module_init();
int  sensor_get_distance_cm();
bool sensor_is_touched();

#endif // ENABLE_MODULE_SENSOR
