#include "event_bus.h"
#include <esp_log.h>

static const char *TAG = "EVENT_BUS";
static QueueHandle_t s_event_queue = nullptr;

bool event_bus_init(uint32_t queue_size) {
    if (s_event_queue != nullptr) {
        return true; // Đã khởi tạo trước đó
    }

    s_event_queue = xQueueCreate(queue_size, sizeof(SystemEvent));
    if (s_event_queue == nullptr) {
        ESP_LOGE(TAG, "Khoi tao FreeRTOS Event Queue that bai!");
        return false;
    }

    ESP_LOGI(TAG, "Khoi tao Event Queue thanh cong voi dung luong %u su kien", queue_size);
    return true;
}

bool event_bus_post(const SystemEvent &event, uint32_t timeout_ms) {
    if (s_event_queue == nullptr) {
        return false;
    }

    TickType_t ticks = (timeout_ms == 0) ? 0 : pdMS_TO_TICKS(timeout_ms);
    BaseType_t res = xQueueSend(s_event_queue, &event, ticks);
    return (res == pdTRUE);
}

bool event_bus_post_type(EventType type, uint32_t timeout_ms) {
    SystemEvent evt;
    evt.type = type;
    evt.param.ptr = nullptr;
    evt.timestamp = millis();
    return event_bus_post(evt, timeout_ms);
}

bool event_bus_receive(SystemEvent *event, TickType_t timeout_ticks) {
    if (s_event_queue == nullptr || event == nullptr) {
        return false;
    }

    BaseType_t res = xQueueReceive(s_event_queue, event, timeout_ticks);
    return (res == pdTRUE);
}
