#include "CameraService.h"
#include "app_config.h"
#include "camera_pins.h"
#include <EventBus.h>
#include <esp_camera.h>
#include <esp_err.h>
#include <FS.h>
#include <SD_MMC.h>

static const uint8_t FRAME_MAGIC_HEADER[4] = {0xAA, 0x55, 0xAA, 0x55};
static const uint8_t FRAME_MAGIC_FOOTER[2] = {0x55, 0xAA};

static bool s_sd_card_ready = false;
static int s_photo_counter = 0;
static volatile bool s_capture_requested = false;
static TaskHandle_t s_camera_task_handle = nullptr;

#if ENABLE_MODULE_SD_CARD
static bool init_sd_card() {
    SD_MMC.setPins(PIN_SD_MMC_CLK, PIN_SD_MMC_CMD, PIN_SD_MMC_D0);
    if (!SD_MMC.begin("/sdcard", true)) {
        return false;
    }
    return (SD_MMC.cardType() != CARD_NONE);
}

static bool save_photo_to_sd(camera_fb_t *fb) {
    if (!s_sd_card_ready || !fb) return false;

    s_photo_counter++;
    char filename[32];
    snprintf(filename, sizeof(filename), "/photo_%04d.jpg", s_photo_counter);

    File file = SD_MMC.open(filename, FILE_WRITE);
    if (!file) return false;

    size_t written = file.write(fb->buf, fb->len);
    file.close();
    return (written == fb->len);
}
#endif

bool CameraService::init() {
#if ENABLE_MODULE_SD_CARD
    s_sd_card_ready = init_sd_card();
#endif

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.grab_mode = CAMERA_GRAB_LATEST;
    config.fb_location = CAMERA_FB_IN_PSRAM;

    if (psramFound()) {
        config.frame_size = FRAMESIZE_VGA;
        config.jpeg_quality = 12;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 15;
        config.fb_count = 1;
        config.fb_location = CAMERA_FB_IN_DRAM;
    }

    return (esp_camera_init(&config) == ESP_OK);
}

void CameraService::requestCapture() {
    s_capture_requested = true;
}

static void camera_task_worker(void *param) {
    while (true) {
        while (Serial.available() > 0) {
            char cmd = (char)Serial.read();
            sensor_t *s = esp_camera_sensor_get();
            if (cmd == 'c' || cmd == 's') {
                EventBus::postType(EVENT_CAMERA_CAPTURE_REQUEST);
            } else if (cmd == '1' && s != nullptr) {
                s->set_framesize(s, FRAMESIZE_QVGA);
            } else if (cmd == '2' && s != nullptr) {
                s->set_framesize(s, FRAMESIZE_VGA);
            } else if (cmd == '3' && s != nullptr) {
                s->set_framesize(s, FRAMESIZE_SVGA);
            }
        }

        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        Serial.write(FRAME_MAGIC_HEADER, 4);
        uint32_t len = fb->len;
        Serial.write((const uint8_t*)&len, 4);
        Serial.write(fb->buf, fb->len);
        Serial.write(FRAME_MAGIC_FOOTER, 2);

        if (s_capture_requested) {
#if ENABLE_MODULE_SD_CARD
            save_photo_to_sd(fb);
#endif
            s_capture_requested = false;
        }

        esp_camera_fb_return(fb);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

bool CameraService::startTask(uint8_t core_id) {
    BaseType_t res = xTaskCreatePinnedToCore(
        camera_task_worker,
        "CameraTask",
        4096,
        nullptr,
        2,
        &s_camera_task_handle,
        core_id
    );
    return (res == pdPASS);
}
