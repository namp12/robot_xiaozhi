#include "SystemBrain.h"
#include "app_config.h"
#include <EventBus.h>
#include <CameraService.h>
#include <MotorDriver.h>
#include <DisplayService.h>
#include <AudioService.h>
#include <SensorService.h>
#include <esp_log.h>

static const char *TAG = "SYSTEM_BRAIN";
static TaskHandle_t s_brain_task_handle = nullptr;

static void brain_task_worker(void *param) {
    ESP_LOGI(TAG, "SystemBrain da khoi dong va san sang xu ly su kien tren Core %d", xPortGetCoreID());
    SystemEvent evt;

    while (true) {
        if (EventBus::receive(&evt, portMAX_DELAY)) {
            switch (evt.type) {
                case EVENT_SYSTEM_BOOT_COMPLETED:
                    ESP_LOGI(TAG, "[EVENT] He thong khoi dong thanh cong!");
                    DisplayService::setEmotion(EMOTION_NORMAL);
                    break;

                case EVENT_CAMERA_CAPTURE_REQUEST:
                    ESP_LOGI(TAG, "[EVENT] Chup va luu anh SD");
                    CameraService::requestCapture();
                    break;

                case EVENT_TOUCH_PRESSED:
                    ESP_LOGI(TAG, "[EVENT] Cham vao dau robot!");
                    DisplayService::setEmotion(EMOTION_HAPPY);
                    AudioService::playSound("happy.mp3");
                    break;

                case EVENT_DISTANCE_OBSTACLE:
                    ESP_LOGW(TAG, "[EVENT] Vat can o khoang cach: %d cm", evt.param.i32);
                    MotorDriver::stop();
                    DisplayService::setEmotion(EMOTION_SURPRISED);
                    break;

                case EVENT_MOTOR_MOVE_FORWARD:
                    MotorDriver::forward(evt.param.i32 > 0 ? evt.param.i32 : 100);
                    break;

                case EVENT_MOTOR_STOP:
                    MotorDriver::stop();
                    break;

                default:
                    ESP_LOGD(TAG, "[EVENT] Su kien chua xu ly: %d", evt.type);
                    break;
            }
        }
    }
}

bool SystemBrain::init(uint8_t core_id) {
#if ENABLE_MODULE_BRAIN
    BaseType_t res = xTaskCreatePinnedToCore(
        brain_task_worker,
        "BrainTask",
        4096,
        nullptr,
        1,
        &s_brain_task_handle,
        core_id
    );
    return (res == pdPASS);
#else
    return false;
#endif
}
