# 📡 Component `SensorService` - Cảm Biến Khoảng Cách ToF & Chạm

Component `SensorService` quản lý các giác quan tiếp nhận môi trường của Robot Xiaozhi, bao gồm cảm biến đo khoảng cách bằng tia laser ToF (Time-of-Flight) và nút cảm ứng chạm điện dung.

---

## 📂 Các file trong component
* [`SensorService.h`](SensorService.h): Khai báo hàm khởi tạo và các hàm đọc cảm biến `getDistanceCm()`, `isTouched()`.
* [`SensorService.cpp`](SensorService.cpp): Hiện thực chi tiết đọc ngoại vi.

---

## 🔌 Chi tiết phần cứng và kết nối

### 1. Cảm biến khoảng cách Laser ToF (VL53L0X / VL53L1X)
* **Nguyên lý hoạt động**: Phát tia photon laser hồng ngoại vô hại (940nm) và đo thời gian phản xạ trở lại để tính khoảng cách chính xác từng milimet, không bị ảnh hưởng bởi màu sắc hay bề mặt vật thể như cảm biến siêu âm SR04.
* **Giao tiếp**: Sử dụng chung đường I2C với màn hình OLED:
  * **SDA**: `GPIO 41`
  * **SCL**: `GPIO 42`
  * **Địa chỉ I2C**: `0x29`
* **Ứng dụng trên robot**:
  * Đặt phía trước mũi xe để phát hiện chướng ngại vật chống va chạm.
  * Hướng chếch xuống mặt bàn để phát hiện mép vực chống rơi bàn khi robot chạy tự hành.

### 2. Cảm biến chạm điện dung (TTP223 Mini)
* **Nguyên lý hoạt động**: Nhận diện sự thay đổi điện dung khi có ngón tay người tiếp xúc gần hoặc chạm vào bề mặt. Có thể giấu ẩn bên dưới lớp vỏ nhựa in 3D trên đỉnh đầu robot.
* **Chân tín hiệu I/O**: Nối vào `GPIO 45` (`PIN_TOUCH_SENSOR` trong `app_config.h`).
* **Ứng dụng trên robot**: Khi người dùng xoa đầu robot, robot sẽ phát hiện và kích hoạt sự kiện `EVENT_TOUCH_PRESSED` để bắt đầu lắng nghe hoặc chào hỏi thân thiện.

---

## 🚀 Cách sử dụng trong code
```cpp
#include <SensorService.h>

void setup() {
    SensorService::init();
}

// Kiểm tra xem người dùng có đang chạm đầu không
if (SensorService::isTouched()) {
    // Có người đang xoa đầu
}

// Lấy khoảng cách vật cản phía trước tính bằng centimet
int distance = SensorService::getDistanceCm();
if (distance > 0 && distance < 10) {
    // Vật cản quá gần (< 10cm) -> Cần phanh gấp!
}
```
