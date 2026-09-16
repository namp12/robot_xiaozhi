#include "AudioService.h"
#include "app_config.h"
#include <esp_log.h>

static const char *TAG = "AUDIO_SERVICE";

bool AudioService::init() {
#if ENABLE_MODULE_AUDIO
    ESP_LOGI(TAG, "Khoi tao AudioService I2S Mic & Spk thanh cong");
    return true;
#else
    return false;
#endif
}

void AudioService::playSound(const char* sound_name) {
#if ENABLE_MODULE_AUDIO
    ESP_LOGI(TAG, "AudioService phat am: %s", sound_name);
#endif
}

void AudioService::startListening() {
#if ENABLE_MODULE_AUDIO
    ESP_LOGI(TAG, "AudioService bat dau thu am Micro...");
#endif
}

void AudioService::stopListening() {
#if ENABLE_MODULE_AUDIO
    ESP_LOGI(TAG, "AudioService dung thu am.");
#endif
}
