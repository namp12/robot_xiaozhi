#include "modules/audio_module.h"

#if ENABLE_MODULE_AUDIO
#include <esp_log.h>

static const char *TAG = "AUDIO_MODULE";

bool audio_module_init() {
    ESP_LOGI(TAG, "Khoi tao Audio Module: Mic I2S (SCK:%d, WS:%d, SD:%d) va Spk (BCLK:%d, LRC:%d, DIN:%d)",
             PIN_I2S_MIC_SCK, PIN_I2S_MIC_WS, PIN_I2S_MIC_SD,
             PIN_I2S_SPK_BCLK, PIN_I2S_SPK_LRC, PIN_I2S_SPK_DIN);
    return true;
}

void audio_play_sound(const char* sound_name) {
    ESP_LOGI(TAG, "Yeu cau phat am thanh: %s", sound_name);
}

void audio_start_listening() {
    ESP_LOGI(TAG, "Bat dau thu am giong noi qua Micro INMP441...");
}

void audio_stop_listening() {
    ESP_LOGI(TAG, "Dung thu am.");
}

#endif // ENABLE_MODULE_AUDIO
