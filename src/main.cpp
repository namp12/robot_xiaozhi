#include <Arduino.h>
#include <esp_err.h>
#include <FS.h>
#include <SD_MMC.h>
#include "esp_camera.h"
#include "camera_pins.h"

// Cấu hình chân MicroSD (Chuẩn SD_MMC 1-bit cho board ESP32-S3 Freenove / Generic)
#define SD_MMC_CLK 39
#define SD_MMC_CMD 38
#define SD_MMC_D0  40

bool sdCardReady = false;
int photoCounter = 0;

// Hàm cấu hình và khởi tạo thẻ nhớ MicroSD
bool initSDCard() {
    Serial.println("[SD] Dang khoi tao the nho MicroSD...");
    SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);

    // Bắt đầu chế độ 1-bit (true)
    if (!SD_MMC.begin("/sdcard", true)) {
        Serial.println("[SD] Khoi tao the nho that bai! (Kiem tra lai: da cam the chua, the dinh dang FAT32/exFAT chua)");
        return false;
    }

    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("[SD] Khong tim thay the nho!");
        return false;
    }

    String typeStr = "Unknown";
    if (cardType == CARD_MMC) typeStr = "MMC";
    else if (cardType == CARD_SD) typeStr = "SDSC";
    else if (cardType == CARD_SDHC) typeStr = "SDHC/SDXC";

    uint64_t totalBytes = SD_MMC.totalBytes() / (1024 * 1024);
    uint64_t usedBytes = SD_MMC.usedBytes() / (1024 * 1024);

    Serial.printf("[SD] The nho hop le! Loai: %s | Dung luong: %llu MB | Da dung: %llu MB\n", 
                  typeStr.c_str(), totalBytes, usedBytes);
    return true;
}

// Hàm lưu buffer ảnh vào file JPEG trên thẻ nhớ
bool savePhotoToSD(camera_fb_t *fb) {
    if (!sdCardReady) {
        Serial.println("[SD] Canh bao: Khong the luu anh vi the nho chua san sang.");
        return false;
    }

    photoCounter++;
    char filename[32];
    snprintf(filename, sizeof(filename), "/photo_%04d.jpg", photoCounter);

    Serial.printf("[SD] Dang ghi anh vao file %s (%u bytes)...\n", filename, (unsigned int)fb->len);

    File file = SD_MMC.open(filename, FILE_WRITE);
    if (!file) {
        Serial.printf("[SD] Loi: Khong the mo file %s de ghi!\n", filename);
        return false;
    }

    size_t written = file.write(fb->buf, fb->len);
    file.close();

    if (written == fb->len) {
        Serial.printf("[SD] >>> DA LUU ANH THANH CONG: %s <<<\n", filename);
        return true;
    } else {
        Serial.printf("[SD] Ghi anh loi: Chi ghi duoc %u / %u bytes!\n", (unsigned int)written, (unsigned int)fb->len);
        return false;
    }
}

// Hàm cấu hình và khởi tạo Camera
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
    config.pixel_format = PIXFORMAT_JPEG; // Dùng JPEG cho truyền tải ảnh / streaming / AI
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;

    // Cấu hình chất lượng hình ảnh dựa vào PSRAM
    if (psramFound()) {
        Serial.println("[CAM] PSRAM tim thay: Bat do phan giai cao (SVGA)");
        config.frame_size = FRAMESIZE_SVGA; // 800x600
        config.jpeg_quality = 12;            // 10-63 (số càng nhỏ chất lượng càng đẹp)
        config.fb_count = 2;                // 2 frame buffer để xử lý mượt mà
    } else {
        Serial.println("[CAM] CANH BAO: Khong tim thay PSRAM! Giam do phan giai xuong QVGA");
        config.frame_size = FRAMESIZE_QVGA; // 320x240
        config.jpeg_quality = 15;
        config.fb_count = 1;
        config.fb_location = CAMERA_FB_IN_DRAM;
    }

    // Khởi tạo camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("[CAM] Khoi tao camera that bai! Ma loi: 0x%x (%s)\n", err, esp_err_to_name(err));
        return false;
    }

    sensor_t *s = esp_camera_sensor_get();
    if (s != nullptr) {
        // Tùy chỉnh lật ảnh nếu camera bị ngược
        // s->set_vflip(s, 1);
        // s->set_hmirror(s, 1);
    }

    Serial.println("[CAM] Khoi tao Camera thanh cong!");
    return true;
}

void setup() {
    Serial.begin(115200);
    // Chờ cổng USB Serial sẵn sàng
    delay(2000);

    Serial.println("\n=============================================");
    Serial.println("   ESP32-S3-CAM + SD CARD (Robot Xiaozhi)    ");
    Serial.println("=============================================");

    // Kiểm tra thông tin bộ nhớ
    Serial.printf("Chip Model: %s (Rev %d)\n", ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("Flash Size: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
    Serial.printf("Free Heap : %d KB\n", ESP.getFreeHeap() / 1024);

    if (psramFound()) {
        Serial.printf("PSRAM Size: %d MB\n", ESP.getPsramSize() / (1024 * 1024));
        Serial.printf("Free PSRAM: %d KB\n", ESP.getFreePsram() / 1024);
    } else {
        Serial.println("PSRAM: KHONG TIM THAY (Kiem tra lai cau hinh platformio.ini)");
    }
    Serial.println("---------------------------------------------");

    // 1. Khởi tạo Camera
    if (!initCamera()) {
        Serial.println("[ERROR] Vui long kiem tra lai chan cam va module Camera!");
    }

    // 2. Khởi tạo thẻ nhớ MicroSD
    sdCardReady = initSDCard();
    if (!sdCardReady) {
        Serial.println("[ERROR] Khong the luu anh vao the nho! Vui long kiem tra lai the nho.");
    }
}

void loop() {
    static unsigned long lastCaptureTime = 0;

    // Chụp và lưu ảnh mỗi 5 giây
    if (millis() - lastCaptureTime > 5000) {
        lastCaptureTime = millis();

        Serial.println("\n[CAM] Dang chup 1 frame...");
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("[CAM] Chup anh that bai! Khong nhan duoc frame tu camera.");
            return;
        }

        Serial.printf("[CAM] Chup thanh cong! Kich thuoc anh: %u bytes (%dx%d, format: %d)\n", 
                      (unsigned int)fb->len, fb->width, fb->height, fb->format);

        // Lưu ảnh vào thẻ nhớ
        if (sdCardReady) {
            savePhotoToSD(fb);
        } else {
            Serial.println("[SD] The nho chua san sang -> Bo qua luu anh.");
        }

        // Trả lại buffer bộ nhớ cho camera (BẮT BUỘC để tránh tràn RAM)
        esp_camera_fb_return(fb);
    }
}