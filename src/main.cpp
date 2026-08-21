#include <Arduino.h>
#include "esp_camera.h"
#include "camera_pins.h"

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
        config.frame_size = FRAMESIZE_SVGA; // 800x600 (hoặc FRAMESIZE_UXGA 1600x1200 / FRAMESIZE_HD)
        config.jpeg_quality = 12;            // 10-63 (so cang nho chat luong cang dep)
        config.fb_count = 2;                // 2 frame buffer de stream muot ma
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
        Serial.printf("[CAM] Khoi tao camera that bai! Ma loi: 0x%x\n", err);
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
    // Chờ cổng USB CDC Serial sẵn sàng
    delay(2000);

    Serial.println("\n=============================================");
    Serial.println("   ESP32-S3-CAM KHOI DONG (Robot Xiaozhi)   ");
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

    // Khởi tạo camera
    if (!initCamera()) {
        Serial.println("[ERROR] Vui long kiem tra lai chan cam va module Camera!");
    }
}

void loop() {
    static unsigned long lastCaptureTime = 0;

    // Chụp thử 1 khung hình mỗi 3 giây để kiểm tra
    if (millis() - lastCaptureTime > 3000) {
        lastCaptureTime = millis();

        Serial.println("[CAM] Dang chup thu 1 frame...");
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("[CAM] Chup anh that bai!");
            return;
        }

        Serial.printf("[CAM] Chup thanh cong! Kich thuoc anh: %u bytes (%dx%d, format: %d)\n", 
                      (unsigned int)fb->len, fb->width, fb->height, fb->format);

        // Trả lại buffer bộ nhớ cho camera (BẮT BUỘC để tránh tràn RAM)
        esp_camera_fb_return(fb);
    }
}