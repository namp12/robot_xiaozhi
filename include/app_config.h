#pragma once
#include <Arduino.h>

// ==============================================================================
// 1. CÔNG TẮC BẬT / TẮT CÁC MODULE (1 = BẬT, 0 = TẮT)
// Khi bạn có linh kiện nào, chỉ cần bật 1 cho module đó.
// Hệ thống sẽ tự động khởi chạy Task và lắng nghe Event tương ứng mà không xung đột!
// ==============================================================================
#define ENABLE_MODULE_CAMERA    1   // Module Camera (Đang có sẵn trên mạch ESP32-S3-CAM)
#define ENABLE_MODULE_SD_CARD   1   // Thẻ nhớ MicroSD lưu ảnh
#define ENABLE_MODULE_BRAIN     1   // Não bộ điều phối trung tâm (Xử lý Event Bus)

// Các module mở rộng (Bật khi bạn gắn thêm linh kiện tự chọn)
#define ENABLE_MODULE_DISPLAY   0   // Màn hình biểu cảm (OLED SSD1306 / LCD GC9A01/ST7789)
#define ENABLE_MODULE_MOTOR     0   // Động cơ di chuyển (L298N Mini / DRV8833 + Motor N20)
#define ENABLE_MODULE_AUDIO     0   // Âm thanh I2S (Mic INMP441 + Loa MAX98357A)
#define ENABLE_MODULE_SENSOR    0   // Cảm biến khoảng cách (VL53L0X) + Cảm ứng chạm (TTP223)

// ==============================================================================
// 2. CẤU HÌNH GIAO TIẾP & HỆ THỐNG
// ==============================================================================
#define SYSTEM_SERIAL_BAUD      2000000     // 2Mbaud truyền video thời gian thực siêu mượt
#define EVENT_QUEUE_SIZE        32          // Kích thước hàng đợi sự kiện (chứa tối đa 32 sự kiện)

// Định nghĩa nhân CPU (Core) cho từng tác vụ
#define CORE_HEAVY_MULTIMEDIA   0           // Core 0: Camera, Audio DMA, Stream nặng
#define CORE_BRAIN_AND_CONTROL  1           // Core 1: Não bộ, Xử lý sự kiện, Động cơ, Màn hình

// ==============================================================================
// 3. SƠ ĐỒ CHÂN GPIO TÙY BIẾN CHO CÁC MODULE BẠN SẼ GẮN
// (Bạn có thể tự do thay đổi các chân GPIO dưới đây theo cách hàn của bạn)
// ==============================================================================

// --- Thẻ nhớ MicroSD (SD_MMC 1-bit mode) ---
#define PIN_SD_MMC_CLK          39
#define PIN_SD_MMC_CMD          38
#define PIN_SD_MMC_D0           40

// --- Màn hình OLED / Cảm biến I2C (Chung Bus I2C) ---
#define PIN_I2C_SDA             41
#define PIN_I2C_SCL             42

// --- Động cơ 2 bánh (L298N Mini / Cầu H) ---
#define PIN_MOTOR_LEFT_IN1      2
#define PIN_MOTOR_LEFT_IN2      1
#define PIN_MOTOR_RIGHT_IN3     47
#define PIN_MOTOR_RIGHT_IN4     48

// --- Cảm biến chạm & Cảm biến phụ ---
#define PIN_TOUCH_SENSOR        45

// --- Âm thanh I2S (Micro INMP441) ---
#define PIN_I2S_MIC_SCK         43
#define PIN_I2S_MIC_WS          44
#define PIN_I2S_MIC_SD          1

// --- Âm thanh I2S (Loa MAX98357A) ---
#define PIN_I2S_SPK_BCLK        19
#define PIN_I2S_SPK_LRC         20
#define PIN_I2S_SPK_DIN         21

// --- Đèn LED hiệu ứng ---
#define PIN_LED_WS2812          38
#define PIN_LED_INDICATOR       47
