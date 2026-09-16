#pragma once
#include <Arduino.h>
#include "app_config.h"

#if ENABLE_MODULE_BRAIN

/**
 * Khởi động Não bộ điều phối trung tâm (Task xử lý các sự kiện từ Event Bus)
 * Chạy trên Core 1 để đảm bảo logic tức thời, không bị ảnh hưởng bởi camera/audio
 */
bool system_brain_init();

#endif // ENABLE_MODULE_BRAIN
