# 📦 Thư mục `lib/` - Kiến trúc Component Robot Xiaozhi

Thư mục `lib/` chứa toàn bộ các **Module thư viện độc lập (Private Components)** của Robot Xiaozhi theo tiêu chuẩn của PlatformIO.

Mỗi thư mục con trong đây là một module tự đóng gói (Encapsulated Component), có thể độc lập biên dịch và tái sử dụng cho các dự án ESP32 khác.

---

## 🗂️ Danh sách các Component trong `lib/`

| Component | Tên Class / Header | Vai trò & Trách nhiệm | Nhân chạy (Core) |
| :--- | :--- | :--- | :---: |
| **[`EventBus`](EventBus/README.md)** | `EventBus.h` | Hàng đợi sự kiện trung tâm FreeRTOS, giúp các module trao đổi dữ liệu phi chặn. | Cả 2 Core |
| **[`CameraService`](CameraService/README.md)** | `CameraService.h` | Lấy khung hình OV2640, quản lý PSRAM và stream video USB Serial 2Mbaud. | Core 0 |
| **[`SystemBrain`](SystemBrain/README.md)** | `SystemBrain.h` | Não bộ điều phối trung tâm: lắng nghe Event Bus và ra lệnh cho các ngoại vi. | Core 1 |
| **[`MotorDriver`](MotorDriver/README.md)** | `MotorDriver.h` | Điều khiển mạch cầu H (L298N Mini / DRV8833) lái 2 động cơ giảm tốc N20. | Core 1 |
| **[`DisplayService`](DisplayService/README.md)** | `DisplayService.h` | Hiển thị các biểu cảm mắt (Vui, Buồn, Ngạc nhiên...) trên màn hình OLED/LCD. | Core 1 |
| **[`AudioService`](AudioService/README.md)** | `AudioService.h` | Xử lý âm thanh số I2S: thu âm từ Micro INMP441 và phát âm thanh qua Loa MAX98357A. | Core 0 |
| **[`SensorService`](SensorService/README.md)** | `SensorService.h` | Đọc cảm biến laser khoảng cách VL53L0X (chống va chạm) và nút cảm ứng chạm TTP223. | Core 1 |

---

## 💡 Quy tắc thiết kế Component của dự án
1. **Không chặn (Non-blocking)**: Không dùng các hàm `delay()` lâu trong các hàm của component. Mọi tác vụ có độ trễ phải chạy trong Task FreeRTOS riêng hoặc dùng `vTaskDelay()`.
2. **Giao tiếp qua Event Bus**: Các module không gọi trực tiếp lẫn nhau (ví dụ: `SensorService` không gọi trực tiếp `MotorDriver`). Khi có biến cố, module chỉ cần bắn một sự kiện vào `EventBus`, sau đó `SystemBrain` sẽ phân phối lệnh xử lý.
3. **Tuân thủ cờ `ENABLE_MODULE_...`**: Mỗi module luôn được bọc trong điều kiện tiền xử lý `#if ENABLE_MODULE_...` từ [`include/app_config.h`](../include/app_config.h). Khi tắt, module sẽ không tiêu tốn RAM hay chân GPIO.
