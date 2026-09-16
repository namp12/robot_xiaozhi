# 📬 Component `EventBus` - Hàng Đợi Sự Kiện FreeRTOS

Thư viện `EventBus` là **trái tim giao tiếp trung tâm** của Robot Xiaozhi. Thư viện này bọc lại cơ chế `xQueue` của hệ điều hành thời gian thực FreeRTOS thành một giao diện C++ an toàn đa luồng (thread-safe), cho phép các tác vụ trên **Core 0** và **Core 1** trao đổi thông tin phi chặn (non-blocking).

---

## 📂 Các file trong component
* [`EventBus.h`](EventBus.h): Định nghĩa danh mục sự kiện `EventType`, cấu trúc gói tin `SystemEvent` và lớp API `EventBus`.
* [`EventBus.cpp`](EventBus.cpp): Triển khai các hàm tạo hàng đợi, đẩy sự kiện (`post`) và lấy sự kiện (`receive`).

---

## 📑 Danh mục các loại sự kiện (`EventType`)

| Nhóm | Tên Event | Ý nghĩa | Dữ liệu kèm theo (`param`) |
| :--- | :--- | :--- | :--- |
| **Hệ thống** | `EVENT_SYSTEM_BOOT_COMPLETED` | Tất cả các module đã khởi động xong. | Không |
| | `EVENT_SYSTEM_LOW_BATTERY` | Cảnh báo điện áp pin xuống thấp. | Điện áp thực tế (`f32`) |
| **Thị giác** | `EVENT_CAMERA_CAPTURE_REQUEST` | Yêu cầu chụp ảnh lưu thẻ nhớ SD. | Không |
| | `EVENT_CAMERA_FRAME_READY` | Đã có một frame ảnh JPEG mới sẵn sàng. | Con trỏ Frame Buffer (`ptr`) |
| | `EVENT_CAMERA_FACE_DETECTED` | Phát hiện khuôn mặt người trong tầm nhìn. | Tọa độ khuôn mặt (`ptr`) |
| **Cảm biến** | `EVENT_TOUCH_PRESSED` | Người dùng chạm vào đầu robot (TTP223). | Không |
| | `EVENT_TOUCH_RELEASED` | Nhả chạm khỏi đầu robot. | Không |
| | `EVENT_DISTANCE_OBSTACLE` | Cảm biến laser ToF phát hiện vật cản phía trước. | Khoảng cách tính bằng cm (`i32`) |
| | `EVENT_DISTANCE_CLIFF` | Phát hiện mép bàn/vực cao có nguy cơ rơi. | Không |
| **Động cơ** | `EVENT_MOTOR_MOVE_FORWARD` | Lệnh cho xe chạy tiến. | Tốc độ PWM 0-255 (`i32`) |
| | `EVENT_MOTOR_MOVE_BACKWARD` | Lệnh cho xe chạy lùi. | Tốc độ PWM 0-255 (`i32`) |
| | `EVENT_MOTOR_TURN_LEFT` | Lệnh quay trái. | Tốc độ quay (`i32`) |
| | `EVENT_MOTOR_TURN_RIGHT` | Lệnh quay phải. | Tốc độ quay (`i32`) |
| | `EVENT_MOTOR_STOP` | Lệnh phanh dừng toàn bộ động cơ. | Không |
| **Biểu cảm** | `EVENT_UI_SET_EMOTION_NORMAL` | Chuyển mắt sang trạng thái bình thường/chớp mắt. | Không |
| | `EVENT_UI_SET_EMOTION_HAPPY` | Chuyển mắt sang trạng thái cười vui vẻ. | Không |
| | `EVENT_UI_SET_EMOTION_SAD` | Chuyển mắt sang trạng thái buồn rũ. | Không |
| | `EVENT_UI_SET_EMOTION_SURPRISED`| Chuyển mắt sang trạng thái ngạc nhiên/tròn xoe. | Không |
| | `EVENT_UI_SET_EMOTION_ANGRY` | Chuyển mắt sang trạng thái tức giận. | Không |
| **Âm thanh** | `EVENT_AUDIO_WAKEWORD_DETECTED` | Nhận diện từ khóa đánh thức ("Xiaozhi"). | Không |
| | `EVENT_AUDIO_RECORD_START` | Bắt đầu thu âm giọng nói từ Micro INMP441. | Không |
| | `EVENT_AUDIO_PLAY_TEXT_RESPONSE`| Nhận văn bản trả lời và kích hoạt loa phát. | Con trỏ chuỗi text (`ptr`) |
| **Mạng AI** | `EVENT_WIFI_CONNECTED` | ESP32-S3 đã kết nối mạng WiFi thành công. | Không |
| | `EVENT_AI_RESPONSE_RECEIVED` | Server LLM gửi kết quả phản hồi về. | Con trỏ JSON/Text (`ptr`) |

---

## 🛠️ Hướng dẫn sử dụng API

### 1. Khởi tạo EventBus (Trong `setup()`):
```cpp
#include <EventBus.h>

void setup() {
    // Tạo hàng đợi chứa tối đa 32 sự kiện
    EventBus::init(32);
}
```

### 2. Bắn một sự kiện đơn giản (Producer):
```cpp
// Bắn sự kiện không cần dữ liệu kèm theo (rất nhanh, không chờ đợi)
EventBus::postType(EVENT_TOUCH_PRESSED);
```

### 3. Bắn một sự kiện kèm dữ liệu:
```cpp
SystemEvent evt;
evt.type = EVENT_DISTANCE_OBSTACLE;
evt.param.i32 = 12; // Khoảng cách 12cm
evt.timestamp = millis();

// Gửi vào hàng đợi với thời gian chờ tối đa 10ms
EventBus::post(evt, 10);
```

### 4. Lắng nghe và lấy sự kiện (Consumer - Trong `SystemBrain`):
```cpp
SystemEvent evt;
// portMAX_DELAY: Task sẽ tự động ngủ để tiết kiệm CPU cho đến khi có sự kiện xuất hiện
if (EventBus::receive(&evt, portMAX_DELAY)) {
    switch (evt.type) {
        case EVENT_TOUCH_PRESSED:
            // Xử lý khi có người chạm đầu
            break;
    }
}
```
