#pragma once
#include <Arduino.h>
#include "app_config.h"

#if ENABLE_MODULE_CAMERA

/**
 * Khởi tạo phần cứng Camera và Thẻ nhớ SD (nếu được bật)
 * @return true nếu camera sẵn sàng hoạt động
 */
bool camera_module_init();

/**
 * Khởi chạy Task Camera chạy độc lập trên Core 0
 * Tự động stream video qua USB Serial và tương thích script viewer.py
 */
bool camera_module_start_task();

/**
 * Yêu cầu chụp ảnh lưu vào thẻ nhớ SD
 */
void camera_module_request_capture();

#endif // ENABLE_MODULE_CAMERA
