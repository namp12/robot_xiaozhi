# ⚙️ Thư mục `include/` - Cấu hình Toàn cục Dự án Robot Xiaozhi

Thư mục này chứa các file khai báo header cấu hình dùng chung cho toàn bộ dự án firmware, có thể được include bởi `src/main.cpp` cũng như tất cả các thư viện trong `lib/`.

---

## 📂 Danh sách các file cấu hình

### 1. [`app_config.h`](app_config.h) - Trung tâm Cấu hình Hệ thống & Bật/Tắt Module
File này là **nơi duy nhất** bạn cần mở khi lắp ráp hoặc thay đổi linh kiện phần cứng cho robot:

* **Công tắc Bật/Tắt module (`ENABLE_MODULE_...`)**:
  * `ENABLE_MODULE_CAMERA` (`1`/`0`): Bật/Tắt camera OV2640.
  * `ENABLE_MODULE_SD_CARD` (`1`/`0`): Bật/Tắt chức năng lưu ảnh chụp vào thẻ nhớ MicroSD.
  * `ENABLE_MODULE_BRAIN` (`1`/`0`): Bật/Tắt Não bộ điều phối trung tâm xử lý Event Bus.
  * `ENABLE_MODULE_DISPLAY` (`1`/`0`): Bật/Tắt màn hình biểu cảm OLED / LCD tròn.
  * `ENABLE_MODULE_MOTOR` (`1`/`0`): Bật/Tắt mạch cầu H điều khiển 2 bánh xe N20.
  * `ENABLE_MODULE_AUDIO` (`1`/`0`): Bật/Tắt hệ thống âm thanh I2S (Micro & Loa).
  * `ENABLE_MODULE_SENSOR` (`1`/`0`): Bật/Tắt cảm biến khoảng cách ToF và cảm ứng chạm.

* **Bảng chân GPIO tùy biến**:
  Bạn có thể đổi số chân GPIO tương ứng theo cách đi dây hoặc mối hàn thực tế của bạn:
  * Chân động cơ trái/phải (`PIN_MOTOR_...`)
  * Chân giao tiếp I2C chung màn hình & ToF (`PIN_I2C_SDA`, `PIN_I2C_SCL`)
  * Chân micro I2S (`PIN_I2S_MIC_...`)
  * Chân loa I2S (`PIN_I2S_SPK_...`)
  * Chân cảm ứng chạm (`PIN_TOUCH_SENSOR`)
  * Chân LED trang trí (`PIN_LED_...`)

* **Phân bổ nhân CPU (Dual-Core Assignment)**:
  * `CORE_HEAVY_MULTIMEDIA` (Mặc định: **Core 0**): Chạy các tác vụ nặng ngốn CPU (Camera Frame Buffer, I2S DMA, Stream mạng).
  * `CORE_BRAIN_AND_CONTROL` (Mặc định: **Core 1**): Chạy Não bộ logic, xử lý Event Bus, điều khiển động cơ và màn hình.

---

### 2. [`camera_pins.h`](camera_pins.h) - Định nghĩa Chân Phần cứng Cảm biến Camera
File này chứa các định nghĩa chân socket FPC kết nối giữa cảm biến Camera (OV2640/OV5640) và vi điều khiển ESP32-S3:

* Mặc định sử dụng cấu hình: `#define CAMERA_MODEL_ESP32S3_CAM_GENERIC` (Tương thích module ESP32-S3-CAM Freenove, Ai-Thinker, DB-Robot).
* Hỗ trợ các mạch khác khi đổi định nghĩa:
  * `CAMERA_MODEL_XIAO_ESP32S3`: Mạch Seeed Studio XIAO ESP32S3 Sense.
  * `CAMERA_MODEL_ESP32S3_EYE`: Mạch phát triển chính hãng ESP32-S3-EYE của Espressif.
