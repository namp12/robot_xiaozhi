#include "EventBus.h"
#include <esp_log.h>

static const char *TAG = "EVENT_BUS";
static QueueHandle_t s_queue = nullptr;

bool EventBus::init(uint32_t queue_size) {
    if (s_queue != nullptr) {
        return true;
    }
    s_queue = xQueueCreate(queue_size, sizeof(SystemEvent));
    if (s_queue == nullptr) {
        ESP_LOGE(TAG, "Khoi tao FreeRTOS Event Queue that bai!");
        return false;
    }
    ESP_LOGI(TAG, "EventBus san sang (Dung luong: %u su kien)", queue_size);
    return true;
}

bool EventBus::post(const SystemEvent &event, uint32_t timeout_ms) {
    if (s_queue == nullptr) return false;
    TickType_t ticks = (timeout_ms == 0) ? 0 : pdMS_TO_TICKS(timeout_ms);
    return (xQueueSend(s_queue, &event, ticks) == pdTRUE);
}

bool EventBus::postType(EventType type, uint32_t timeout_ms) {
    SystemEvent evt;
    evt.type = type;
    evt.param.ptr = nullptr;
    evt.timestamp = millis();
    return EventBus::post(evt, timeout_ms);
}

bool EventBus::receive(SystemEvent *event, TickType_t timeout_ticks) {
    if (s_queue == nullptr || event == nullptr) return false;
    return (xQueueReceive(s_queue, event, timeout_ticks) == pdTRUE);
}
