/**
 * @file WebController.h
 * @brief Web Controller & MJPEG Video Streaming Service cho Robot Xiaozhi
 * @author Phuong Nam (phuongnam)
 * @copyright (c) 2026 Phuong Nam. All rights reserved.
 * @note Bản quyền thuộc về Phương Nam (phuongnam). Nghiêm cấm sao chép khi chưa được phép.
 */

#pragma once
#include <Arduino.h>

class WebController {
public:
    /**
     * @brief Khởi tạo mạng WiFi (AP/STA) và các máy chủ HTTP Server / Stream Server
     * @return true nếu khởi tạo thành công
     */
    static bool init();

    /**
     * @brief Bắt đầu FreeRTOS task phục vụ Web và Stream video
     * @param core_id Core CPU thực thi (Core 0 cho mạng và stream nặng)
     * @return true nếu tạo task thành công
     */
    static bool startTask(uint8_t core_id = 0);

    /**
     * @brief Lấy địa chỉ IP hiện tại của Robot
     */
    static String getIP();

    /**
     * @brief Kiểm tra robot đang chạy ở chế độ Access Point hay Station
     */
    static bool isAPMode();
};
