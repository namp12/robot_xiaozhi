#include "modules/display_module.h"

#if ENABLE_MODULE_DISPLAY
#include <esp_log.h>

static const char *TAG = "DISPLAY_MODULE";

bool display_module_init() {
    ESP_LOGI(TAG, "Khoi tao Display Module (I2C OLED/LCD) tren chan SDA:%d, SCL:%d", PIN_I2C_SDA, PIN_I2C_SCL);
    // Khi ban lap man hinh (SSD1306/GC9A01), them code khoi tao thu vien tai day
    return true;
}

void display_set_emotion(RobotEmotion emotion) {
    ESP_LOGI(TAG, "Doi bieu cam mat sang: %d", emotion);
    // Khi ban lap man hinh, them ham ve mat tuong ung tai day
}

#endif // ENABLE_MODULE_DISPLAY
