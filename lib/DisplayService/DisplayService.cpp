#include "DisplayService.h"
#include "app_config.h"
#include <esp_log.h>

static const char *TAG = "DISPLAY_SERVICE";

bool DisplayService::init() {
#if ENABLE_MODULE_DISPLAY
    ESP_LOGI(TAG, "Khoi tao DisplayService tren SDA:%d, SCL:%d", PIN_I2C_SDA, PIN_I2C_SCL);
    return true;
#else
    return false;
#endif
}

void DisplayService::setEmotion(RobotEmotion emotion) {
#if ENABLE_MODULE_DISPLAY
    ESP_LOGI(TAG, "Cap nhat bieu cam mat: %d", emotion);
#endif
}
