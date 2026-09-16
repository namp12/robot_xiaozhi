# 🎙️ Component `AudioService` - Âm Thanh Số I2S (Micro & Loa)

Component `AudioService` chịu trách nhiệm thu phát âm thanh số chuẩn **I2S (Inter-IC Sound)** với độ trễ thấp và chất lượng cao, phục vụ cho tính năng trò chuyện giọng nói với AI (Xiaozhi Voice Interaction).

---

## 📂 Các file trong component
* [`AudioService.h`](AudioService.h): Khai báo các hàm điều khiển phát âm thanh, bắt đầu thu âm và dừng thu âm.
* [`AudioService.cpp`](AudioService.cpp): Hiện thực chi tiết cấu hình giao tiếp I2S DMA của ESP32-S3.

---

## 🔌 Phần cứng và Sơ đồ chân kết nối

Hệ thống âm thanh của robot sử dụng 2 module riêng biệt:

### 1. Micro số I2S (INMP441 - Thu âm giọng nói)
* **SCK / BCLK (Bit Clock)**: `GPIO 43` (`PIN_I2S_MIC_SCK`)
* **WS / LRC (Word Select / Left-Right Clock)**: `GPIO 44` (`PIN_I2S_MIC_WS`)
* **SD (Serial Data Out)**: `GPIO 1` (`PIN_I2S_MIC_SD`)
* **L/R (Left/Right Channel Select)**: Nối xuống **GND** (Chọn kênh âm thanh Trái).

### 2. Mạch khuếch đại Loa I2S (MAX98357A - Phát âm thanh)
* **BCLK (Bit Clock)**: `GPIO 19` (`PIN_I2S_SPK_BCLK`)
* **LRC (Left-Right Clock)**: `GPIO 20` (`PIN_I2S_SPK_LRC`)
* **DIN (Serial Data In)**: `GPIO 21` (`PIN_I2S_SPK_DIN`)
* **GAIN**: Nối GND (Mức tăng mặc định +9dB).
* **Nguồn VIN**: Nối nguồn **5V** (cho âm lượng loa to và trong trẻo nhất).

---

## 🚀 Cách sử dụng trong code
```cpp
#include <AudioService.h>

void setup() {
    AudioService::init();
}

// Bắt đầu thu âm giọng người nói để gửi lên server AI
AudioService::startListening();

// Dừng thu âm
AudioService::stopListening();

// Phát một file âm thanh hiệu ứng hoặc phản hồi
AudioService::playSound("greeting.mp3");
```
