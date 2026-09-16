#include <Arduino.h>
#include "app_config.h"

// Các thư viện thành phần từ thư mục lib/
#include <EventBus.h>
#include <CameraService.h>
#include <SystemBrain.h>
#include <MotorDriver.h>
#include <DisplayService.h>
#include <AudioService.h>
#include <SensorService.h>

#include <esp_log.h>

static const char *TAG = "ROBOT_MAIN";

void setup() {
    // 1. Khởi tạo USB Serial tốc độ cao
    Serial.begin(SYSTEM_SERIAL_BAUD);
    Serial.setRxBufferSize(256);
    delay(1000);

    ESP_LOGI(TAG, "=================================================");
    ESP_LOGI(TAG, "🤖 ROBOT XIAOZHI - MODULAR ARCHITECTURE (LIB)");
    ESP_LOGI(TAG, "Chip: %s | Cores: %d", ESP.getChipModel(), ESP.getChipCores());
    ESP_LOGI(TAG, "Free SRAM: %u bytes", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    if (psramFound()) {
        ESP_LOGI(TAG, "PSRAM OPI Found: %u bytes (OK)", ESP.getPsramSize());
    } else {
        ESP_LOGW(TAG, "CANH BAO: PSRAM khong tim thay!");
    }
    ESP_LOGI(TAG, "=================================================");

    // 2. Khởi tạo Hàng đợi sự kiện EventBus
    if (!EventBus::init(EVENT_QUEUE_SIZE)) {
        ESP_LOGE(TAG, "EventBus init failed! Dừng hệ thống.");
        return;
    }

    // 3. Khởi tạo các Component theo cấu hình app_config.h
#if ENABLE_MODULE_CAMERA
    if (CameraService::init()) {
        CameraService::startTask(CORE_HEAVY_MULTIMEDIA); // Core 0
        ESP_LOGI(TAG, "CameraService running on Core %d", CORE_HEAVY_MULTIMEDIA);
    } else {
        ESP_LOGE(TAG, "CameraService init failed!");
    }
#endif

#if ENABLE_MODULE_MOTOR
    MotorDriver::init();
#endif

#if ENABLE_MODULE_DISPLAY
    DisplayService::init();
#endif

#if ENABLE_MODULE_AUDIO
    AudioService::init();
#endif

#if ENABLE_MODULE_SENSOR
    SensorService::init();
#endif

#if ENABLE_MODULE_BRAIN
    SystemBrain::init(CORE_BRAIN_AND_CONTROL); // Core 1
    ESP_LOGI(TAG, "SystemBrain running on Core %d", CORE_BRAIN_AND_CONTROL);
#endif

    // 4. Bắn sự kiện khởi động hoàn tất
    EventBus::postType(EVENT_SYSTEM_BOOT_COMPLETED);
    ESP_LOGI(TAG, "He thong san sang hoat dong!");
}

void loop() {
    // Giám sát tài nguyên hệ thống (Heartbeat) mỗi 30s
    static uint32_t last_check = 0;
    if (millis() - last_check > 30000) {
        last_check = millis();
        ESP_LOGD(TAG, "Heartbeat | RAM Free: %u | PSRAM Free: %u",
                 ESP.getFreeHeap(),
                 ESP.getFreePsram());
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
}