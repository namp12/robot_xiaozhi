#include "modules/system_brain.h"

#if ENABLE_MODULE_BRAIN

#include "event_bus.h"
#include "modules/camera_module.h"
#include "modules/motor_module.h"
#include "modules/display_module.h"
#include "modules/audio_module.h"
#include "modules/sensor_module.h"
#include <esp_log.h>

static const char *TAG = "ROBOT_BRAIN";
static TaskHandle_t s_brain_task_handle = nullptr;

static void system_brain_task_worker(void *param) {
    ESP_LOGI(TAG, "Nao bo Robot da san sang lang nghe su kien!");
    SystemEvent evt;

    while (true) {
        // Chờ sự kiện từ Event Bus (chế độ tiết kiệm CPU portMAX_DELAY)
        if (event_bus_receive(&evt, portMAX_DELAY)) {
            switch (evt.type) {
                case EVENT_SYSTEM_BOOT_COMPLETED:
                    ESP_LOGI(TAG, "[EVENT] He thong khoi dong hoan tat.");
#if ENABLE_MODULE_DISPLAY
                    display_set_emotion(EMOTION_NORMAL);
#endif
                    break;

                case EVENT_CAMERA_CAPTURE_REQUEST:
                    ESP_LOGI(TAG, "[EVENT] Nhan lenh chup anh luu the nho SD");
#if ENABLE_MODULE_CAMERA
                    camera_module_request_capture();
#endif
                    break;

                case EVENT_TOUCH_PRESSED:
                    ESP_LOGI(TAG, "[EVENT] Nguoi dung xoa dau robot!");
#if ENABLE_MODULE_DISPLAY
                    display_set_emotion(EMOTION_HAPPY);
#endif
#if ENABLE_MODULE_AUDIO
                    audio_play_sound("happy.mp3");
#endif
                    break;

                case EVENT_DISTANCE_OBSTACLE:
                    ESP_LOGW(TAG, "[EVENT] Phat hien vat can o khoang cach: %d cm", evt.param.i32);
#if ENABLE_MODULE_MOTOR
                    motor_stop();
#endif
#if ENABLE_MODULE_DISPLAY
                    display_set_emotion(EMOTION_SURPRISED);
#endif
                    break;

                case EVENT_MOTOR_MOVE_FORWARD:
#if ENABLE_MODULE_MOTOR
                    motor_forward(evt.param.i32 > 0 ? evt.param.i32 : 100);
#endif
                    break;

                case EVENT_MOTOR_STOP:
#if ENABLE_MODULE_MOTOR
                    motor_stop();
#endif
                    break;

                case EVENT_UI_SET_EMOTION_HAPPY:
#if ENABLE_MODULE_DISPLAY
                    display_set_emotion(EMOTION_HAPPY);
#endif
                    break;

                default:
                    ESP_LOGD(TAG, "[EVENT] Su kien chua xu ly: %d", evt.type);
                    break;
            }
        }
    }
}

bool system_brain_init() {
    BaseType_t res = xTaskCreatePinnedToCore(
        system_brain_task_worker,
        "SystemBrainTask",
        4096,
        nullptr,
        1,                       // Priority 1
        &s_brain_task_handle,
        CORE_BRAIN_AND_CONTROL   // Chạy trên Core 1
    );
    return (res == pdPASS);
}

#endif // ENABLE_MODULE_BRAIN
