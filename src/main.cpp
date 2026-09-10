#include <Arduino.h>
#include <esp_err.h>
#include <FS.h>
#include <SD_MMC.h>
#include "esp_camera.h"
#include "camera_pins.h"

// ================================================================
// CẤU HÌNH PHẦN CỨNG
// ================================================================
#define SERIAL_BAUD_RATE 2000000  // 2Mbaud truyền video thời gian thực siêu mượt

// Chân MicroSD (SD_MMC 1-bit mode) cho mạch ESP32-S3 Freenove / Generic
#define SD_MMC_CLK 39
#define SD_MMC_CMD 38
#define SD_MMC_D0  40

// Magic header và footer định danh gói tin video qua Serial
const uint8_t FRAME_MAGIC_HEADER[4] = {0xAA, 0x55, 0xAA, 0x55};
const uint8_t FRAME_MAGIC_FOOTER[2] = {0x55, 0xAA};

bool sdCardReady = false;
int photoCounter = 0;
bool saveToSdRequested = false;

// ================================================================
// KHỞI TẠO THẺ NHỚ
// ================================================================
bool initSDCard() {
    SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
    if (!SD_MMC.begin("/sdcard", true)) {
        return false;
    }
    uint8_t cardType = SD_MMC.cardType();
    return (cardType != CARD_NONE);
}

// Lưu ảnh vào thẻ nhớ SD
bool savePhotoToSD(camera_fb_t *fb) {
    if (!sdCardReady) return false;

    photoCounter++;
    char filename[32];
    snprintf(filename, sizeof(filename), "/photo_%04d.jpg", photoCounter);

    File file = SD_MMC.open(filename, FILE_WRITE);
    if (!file) return false;

    size_t written = file.write(fb->buf, fb->len);
    file.close();

    return (written == fb->len);
}

// ================================================================
// KHỞI TẠO CAMERA
// ================================================================
bool initCamera() {
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
    config.grab_mode = CAMERA_GRAB_LATEST; // Luôn lấy frame mới nhất cho video mượt mà
    config.fb_location = CAMERA_FB_IN_PSRAM;

    if (psramFound()) {
        // VGA 640x480 tối ưu độ nét và tốc độ truyền video qua Serial
        config.frame_size = FRAMESIZE_VGA;
        config.jpeg_quality = 12; // 10-63
        config.fb_count = 2;      // Double buffering
    } else {
        config.frame_size = FRAMESIZE_QVGA; // 320x240
        config.jpeg_quality = 15;
        config.fb_count = 1;
        config.fb_location = CAMERA_FB_IN_DRAM;
    }

    esp_err_t err = esp_camera_init(&config);
    return (err == ESP_OK);
}

// ================================================================
// SETUP
// ================================================================
void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.setRxBufferSize(256);
    delay(1000);

    // Khởi tạo thẻ nhớ (nếu cắm)
    sdCardReady = initSDCard();

    // Khởi tạo camera
    initCamera();
}

// ================================================================
// LOOP: STREAM VIDEO LIÊN TỤC
// ================================================================
void loop() {
    // 1. Kiểm tra lệnh điều khiển gửi từ máy tính qua Serial
    while (Serial.available() > 0) {
        char cmd = (char)Serial.read();
        sensor_t *s = esp_camera_sensor_get();

        if (cmd == 'c' || cmd == 's') {
            // Yêu cầu chụp và lưu ảnh vào thẻ nhớ SD
            saveToSdRequested = true;
        } else if (cmd == '1' && s != nullptr) {
            s->set_framesize(s, FRAMESIZE_QVGA); // 320x240 (FPS cao)
        } else if (cmd == '2' && s != nullptr) {
            s->set_framesize(s, FRAMESIZE_VGA);  // 640x480 (Cân bằng đẹp)
        } else if (cmd == '3' && s != nullptr) {
            s->set_framesize(s, FRAMESIZE_SVGA); // 800x600 (Độ nét cao)
        }
    }

    // 2. Chụp khung hình từ camera
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        delay(10);
        return;
    }

    // 3. Đóng gói và truyền frame qua Serial:
    // [MAGIC HEADER 4B] + [LENGTH 4B] + [JPEG DATA] + [MAGIC FOOTER 2B]
    Serial.write(FRAME_MAGIC_HEADER, 4);

    uint32_t len = fb->len;
    Serial.write((const uint8_t*)&len, 4);

    Serial.write(fb->buf, fb->len);

    Serial.write(FRAME_MAGIC_FOOTER, 2);

    // 4. Nếu có yêu cầu lưu thẻ nhớ
    if (saveToSdRequested) {
        savePhotoToSD(fb);
        saveToSdRequested = false;
    }

    // 5. Giải phóng buffer
    esp_camera_fb_return(fb);
}