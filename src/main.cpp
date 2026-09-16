#include <Arduino.h>
#include "app_config.h"
#include "event_bus.h"
#include "modules/camera_module.h"
#include "modules/system_brain.h"
#include "modules/motor_module.h"
#include "modules/display_module.h"
#include "modules/audio_module.h"
#include "modules/sensor_module.h"
#include <esp_log.h>

static const char *TAG = "ROBOT_MAIN";

void setup() {
    // 1. Khởi tạo USB Serial tốc độ cao
    Serial.begin(SYSTEM_SERIAL_BAUD);
    Serial.setRxBufferSize(256);
    delay(1000);

    ESP_LOGI(TAG, "=================================================");
    ESP_LOGI(TAG, "🤖 ROBOT XIAOZHI - ARCHITECTURE INITIALIZING...");
    ESP_LOGI(TAG, "Chip model: %s, Cores: %d", ESP.getChipModel(), ESP.getChipCores());
    ESP_LOGI(TAG, "Free Internal RAM: %u bytes", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    if (psramFound()) {
        ESP_LOGI(TAG, "PSRAM Found! Size: %u bytes", ESP.getPsramSize());
    } else {
        ESP_LOGW(TAG, "PSRAM khong tim thay! Che do do phan giai thap se duoc su dung.");
    }
    ESP_LOGI(TAG, "=================================================");

    // 2. Khởi tạo Event Bus (Hàng đợi sự kiện FreeRTOS)
    if (!event_bus_init(EVENT_QUEUE_SIZE)) {
        ESP_LOGE(TAG, "Event Bus khoi tao that bai! He thong dung.");
        return;
    }

    // 3. Khởi tạo các module theo cấu hình trong app_config.h
#if ENABLE_MODULE_CAMERA
    if (camera_module_init()) {
        camera_module_start_task(); // Chạy độc lập trên Core 0
        ESP_LOGI(TAG, "Module Camera da khoi dong tren Core %d", CORE_HEAVY_MULTIMEDIA);
    } else {
        ESP_LOGE(TAG, "Module Camera khoi tao that bai!");
    }
#endif

#if ENABLE_MODULE_MOTOR
    motor_module_init();
#endif

#if ENABLE_MODULE_DISPLAY
    display_module_init();
#endif

#if ENABLE_MODULE_AUDIO
    audio_module_init();
#endif

#if ENABLE_MODULE_SENSOR
    sensor_module_init();
#endif

#if ENABLE_MODULE_BRAIN
    system_brain_init(); // Chạy điều phối trên Core 1
    ESP_LOGI(TAG, "Nao bo Robot da khoi dong tren Core %d", CORE_BRAIN_AND_CONTROL);
#endif

    // 4. Phát sự kiện hệ thống đã khởi động hoàn tất
    event_bus_post_type(EVENT_SYSTEM_BOOT_COMPLETED);
    ESP_LOGI(TAG, "Tat ca module da san sang!");
}

void loop() {
    // Vòng loop chính chỉ đảm nhiệm việc theo dõi sức khỏe hệ thống định kỳ (Heartbeat)
    // Toàn bộ logic robot được xử lý tự động qua FreeRTOS Tasks và Event Bus!
    static uint32_t last_check = 0;
    if (millis() - last_check > 30000) { // Mỗi 30 giây
        last_check = millis();
        ESP_LOGD(TAG, "He thong on dinh | RAM trong: %u | PSRAM trong: %u",
                 ESP.getFreeHeap(),
                 ESP.getFreePsram());
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
}