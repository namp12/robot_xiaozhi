#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// ==============================================================================
// 1. DANH SÁCH CÁC LOẠI SỰ KIỆN TOÀN HỆ THỐNG (EVENT TYPE)
// ==============================================================================
enum EventType {
    EVENT_NONE = 0,

    // --- Sự kiện Hệ thống & Nguồn ---
    EVENT_SYSTEM_BOOT_COMPLETED,     // Hệ thống đã khởi động xong tất cả các module
    EVENT_SYSTEM_LOW_BATTERY,        // Pin yếu

    // --- Sự kiện Thị giác (Camera & Image) ---
    EVENT_CAMERA_CAPTURE_REQUEST,    // Yêu cầu chụp ảnh lưu thẻ nhớ
    EVENT_CAMERA_FRAME_READY,        // Đã có 1 frame ảnh mới từ camera
    EVENT_CAMERA_FACE_DETECTED,      // Đã nhận diện khuôn mặt người

    // --- Sự kiện Cảm biến & Tương tác ngoại vi ---
    EVENT_TOUCH_PRESSED,             // Có người chạm vào đầu robot (TTP223)
    EVENT_TOUCH_RELEASED,            // Nhả chạm
    EVENT_DISTANCE_OBSTACLE,         // Phát hiện vật cản phía trước (ToF laser)
    EVENT_DISTANCE_CLIFF,            // Phát hiện vực/mép bàn

    // --- Sự kiện Điều khiển Chuyển động (Motor) ---
    EVENT_MOTOR_MOVE_FORWARD,        // Lệnh tiến
    EVENT_MOTOR_MOVE_BACKWARD,       // Lệnh lùi
    EVENT_MOTOR_TURN_LEFT,           // Lệnh rẽ trái
    EVENT_MOTOR_TURN_RIGHT,          // Lệnh rẽ phải
    EVENT_MOTOR_STOP,                // Lệnh dừng hẳn

    // --- Sự kiện Biểu cảm & Giao diện (Màn hình & LED) ---
    EVENT_UI_SET_EMOTION_NORMAL,     // Mắt bình thường (chớp mắt)
    EVENT_UI_SET_EMOTION_HAPPY,      // Mắt cười vui vẻ
    EVENT_UI_SET_EMOTION_SAD,        // Mắt buồn
    EVENT_UI_SET_EMOTION_SURPRISED,  // Mắt ngạc nhiên/tròn xoe
    EVENT_UI_SET_EMOTION_ANGRY,      // Mắt tức giận

    // --- Sự kiện Âm thanh (Micro & Loa I2S) ---
    EVENT_AUDIO_WAKEWORD_DETECTED,   // Phát hiện từ khóa đánh thức ("Xiaozhi" / "Hey Robot")
    EVENT_AUDIO_RECORD_START,        // Bắt đầu thu âm giọng nói
    EVENT_AUDIO_RECORD_STOP,         // Kết thúc câu nói của người
    EVENT_AUDIO_PLAY_TEXT_RESPONSE,  // Phát câu trả lời từ AI
    EVENT_AUDIO_PLAY_FINISHED,       // Đã phát âm thanh xong

    // --- Sự kiện Mạng & AI Cloud ---
    EVENT_WIFI_CONNECTED,            // Đã kết nối WiFi
    EVENT_WIFI_DISCONNECTED,         // Mất kết nối WiFi
    EVENT_AI_RESPONSE_RECEIVED       // Nhận được text trả lời từ OpenAI / Xiaozhi server
};

// ==============================================================================
// 2. CẤU TRÚC GÓI TIN SỰ KIỆN (SYSTEM EVENT)
// ==============================================================================
struct SystemEvent {
    EventType type;
    union {
        int32_t   i32;               // Giá trị số nguyên (ví dụ: khoảng cách cm, mã lỗi)
        float     f32;               // Giá trị số thực (ví dụ: điện áp pin, tốc độ)
        void*     ptr;               // Con trỏ tới dữ liệu lớn (chuỗi text, frame buffer)
    } param;
    uint32_t timestamp;              // Thời điểm phát sinh sự kiện (millis())
};

// ==============================================================================
// 3. CÁC HÀM GIAO TIẾP EVENT BUS (API)
// ==============================================================================

/**
 * Khởi tạo hàng đợi sự kiện FreeRTOS
 * @param queue_size Số lượng sự kiện tối đa mà hàng đợi có thể chứa
 * @return true nếu khởi tạo thành công
 */
bool event_bus_init(uint32_t queue_size = 32);

/**
 * Đẩy một sự kiện vào Event Bus (Non-blocking, an toàn đa luồng)
 * @param event Gói tin sự kiện cần gửi
 * @param timeout_ms Thời gian chờ tối đa (ms), mặc định = 0 (không chờ)
 * @return true nếu gửi vào hàng đợi thành công
 */
bool event_bus_post(const SystemEvent &event, uint32_t timeout_ms = 0);

/**
 * Đẩy nhanh sự kiện chỉ bằng EventType (tiện dụng cho các sự kiện không cần dữ liệu phụ)
 */
bool event_bus_post_type(EventType type, uint32_t timeout_ms = 0);

/**
 * Lắng nghe và lấy sự kiện ra khỏi Event Bus (dành cho Task điều phối)
 * @param event Con trỏ nhận sự kiện
 * @param timeout_ms Thời gian chờ tối đa (portMAX_DELAY để chờ vĩnh viễn cho đến khi có sự kiện)
 * @return true nếu lấy được sự kiện thành công
 */
bool event_bus_receive(SystemEvent *event, TickType_t timeout_ticks = portMAX_DELAY);
