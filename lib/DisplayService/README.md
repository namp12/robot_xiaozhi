# 👀 Component `DisplayService` - Màn Hình Hiển Thị Biểu Cảm Mắt

Component `DisplayService` phụ trách việc hiển thị khuôn mặt và các biểu cảm cảm xúc (Emotion) của Robot Xiaozhi. Module hỗ trợ cả màn hình OLED (SSD1306 0.96 inch I2C) lẫn màn hình màu LCD tròn (GC9A01 SPI) hoặc LCD chữ nhật (ST7789).

---

## 📂 Các file trong component
* [`DisplayService.h`](DisplayService.h): Khai báo enum cảm xúc `RobotEmotion` và lớp `DisplayService`.
* [`DisplayService.cpp`](DisplayService.cpp): Hiện thực chi tiết hàm khởi tạo màn hình và render biểu cảm mắt.

---

## 😊 Danh sách các trạng thái cảm xúc (`RobotEmotion`)

```cpp
enum RobotEmotion {
    EMOTION_NORMAL,      // Mắt tròn bình thường, tự động nháy mắt (Blink)
    EMOTION_HAPPY,       // Mắt cười híp hình vòng cung (khi được khen hoặc xoa đầu)
    EMOTION_SAD,         // Mắt cụp xuống buồn bã (khi pin yếu hoặc mất kết nối)
    EMOTION_SURPRISED,   // Mắt mở to tròn ngơ ngác (khi gặp vật cản bất ngờ)
    EMOTION_ANGRY        // Mắt vát chéo tức giận (khi bị nhấc bổng hoặc va đập)
};
```

---

## 🔌 Cấu hình kết nối phần cứng (I2C Bus chung)

Màn hình OLED sử dụng chung đường truyền bus I2C với cảm biến khoảng cách laser VL53L0X:
* **SDA**: `GPIO 41` (Cấu hình tại `PIN_I2C_SDA` trong `app_config.h`)
* **SCL**: `GPIO 42` (Cấu hình tại `PIN_I2C_SCL` trong `app_config.h`)
* **Địa chỉ I2C**: Mặc định là `0x3C` (hoặc `0x3D` tùy jumper hàn mặt sau màn hình).

---

## 🚀 Cách sử dụng trong code
```cpp
#include <DisplayService.h>

// Khởi tạo màn hình
DisplayService::init();

// Thay đổi biểu cảm mắt robot
DisplayService::setEmotion(EMOTION_HAPPY);
```
