/**
 * @file WebController.cpp
 * @brief Triển khai Web Controller & MJPEG Streaming cho Robot Xiaozhi
 * @author Phuong Nam (phuongnam)
 * @copyright (c) 2026 Phuong Nam. All rights reserved.
 */

#include "WebController.h"
#include "web_page.h"
#include "app_config.h"
#include <WiFi.h>
#include <WebServer.h>
#include <EventBus.h>
#include <CameraService.h>
#include <esp_log.h>

static const char *TAG = "WEB_CONTROLLER";

static WebServer s_http_server(WEB_SERVER_PORT);
static WiFiServer s_stream_server(STREAM_SERVER_PORT);

static TaskHandle_t s_web_task_handle = nullptr;
static TaskHandle_t s_stream_task_handle = nullptr;
static bool s_is_ap_mode = false;
static bool s_initialized = false;

// Xử lý gửi trang giao diện chính
static void handle_root() {
    s_http_server.send_P(200, "text/html", INDEX_HTML);
}

// Xử lý báo cáo trạng thái hệ thống (Telemetry)
static void handle_status() {
    String json = "{";
    json += "\"ip\":\"" + WebController::getIP() + "\",";
    json += "\"ap\":" + String(s_is_ap_mode ? "true" : "false") + ",";
    json += "\"rssi\":" + String(s_is_ap_mode ? 0 : WiFi.RSSI()) + ",";
    json += "\"heap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"psram\":" + String(ESP.getFreePsram()) + ",";
    json += "\"uptime\":" + String(millis());
    json += "}";
    s_http_server.send(200, "application/json", json);
}

// Chụp 1 khung hình đơn lẻ (Snapshot)
static void handle_capture() {
    camera_fb_t *fb = CameraService::getFrame(200);
    if (!fb) {
        s_http_server.send(503, "text/plain", "Camera busy or unavailable");
        return;
    }

    WiFiClient client = s_http_server.client();
    client.print("HTTP/1.1 200 OK\r\n"
                 "Content-Type: image/jpeg\r\n"
                 "Content-Disposition: inline; filename=capture.jpg\r\n"
                 "Access-Control-Allow-Origin: *\r\n"
                 "Content-Length: ");
    client.println(fb->len);
    client.println();
    client.write(fb->buf, fb->len);

    CameraService::returnFrame(fb);
}

// Xử lý lệnh điều khiển robot từ Web UI
static void handle_control() {
    if (!s_http_server.hasArg("cmd")) {
        s_http_server.send(400, "text/plain", "Thieu tham so cmd");
        return;
    }

    String cmd = s_http_server.arg("cmd");
    int speed = s_http_server.hasArg("speed") ? s_http_server.arg("speed").toInt() : 200;
    if (speed < 0) speed = 0;
    if (speed > 255) speed = 255;

    SystemEvent evt;
    evt.timestamp = millis();

    if (cmd == "forward") {
        evt.type = EVENT_MOTOR_MOVE_FORWARD;
        evt.param.i32 = speed;
        EventBus::post(evt);
        s_http_server.send(200, "text/plain", "FORWARD");
    } else if (cmd == "backward") {
        evt.type = EVENT_MOTOR_MOVE_BACKWARD;
        evt.param.i32 = speed;
        EventBus::post(evt);
        s_http_server.send(200, "text/plain", "BACKWARD");
    } else if (cmd == "left") {
        evt.type = EVENT_MOTOR_TURN_LEFT;
        evt.param.i32 = speed;
        EventBus::post(evt);
        s_http_server.send(200, "text/plain", "LEFT");
    } else if (cmd == "right") {
        evt.type = EVENT_MOTOR_TURN_RIGHT;
        evt.param.i32 = speed;
        EventBus::post(evt);
        s_http_server.send(200, "text/plain", "RIGHT");
    } else if (cmd == "stop") {
        EventBus::postType(EVENT_MOTOR_STOP);
        s_http_server.send(200, "text/plain", "STOPPED");
    } else if (cmd == "capture") {
        EventBus::postType(EVENT_CAMERA_CAPTURE_REQUEST);
        s_http_server.send(200, "text/plain", "CAPTURING_TO_SD");
    } else if (cmd == "emotion") {
        String val = s_http_server.arg("val");
        if (val == "happy") {
            EventBus::postType(EVENT_UI_SET_EMOTION_HAPPY);
        } else if (val == "sad") {
            EventBus::postType(EVENT_UI_SET_EMOTION_SAD);
        } else if (val == "surprised") {
            EventBus::postType(EVENT_UI_SET_EMOTION_SURPRISED);
        } else if (val == "angry") {
            EventBus::postType(EVENT_UI_SET_EMOTION_ANGRY);
        } else {
            EventBus::postType(EVENT_UI_SET_EMOTION_NORMAL);
        }
        s_http_server.send(200, "text/plain", "EMOTION_SET");
    } else if (cmd == "lamp") {
        String val = s_http_server.arg("val");
        pinMode(PIN_LED_INDICATOR, OUTPUT);
        digitalWrite(PIN_LED_INDICATOR, (val == "on") ? HIGH : LOW);
        s_http_server.send(200, "text/plain", val == "on" ? "LAMP_ON" : "LAMP_OFF");
    } else {
        s_http_server.send(400, "text/plain", "Lenh khong hop le");
    }
}

// Task phục vụ HTTP Web Server trên Port 80
static void web_task_worker(void *param) {
    ESP_LOGI(TAG, "WebTask bat dau tren Core %d", xPortGetCoreID());
    while (true) {
        s_http_server.handleClient();
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}

// Task phục vụ MJPEG Streaming trên Port 81 (Hoàn toàn độc lập, không nghẽn lệnh điều khiển)
static void stream_task_worker(void *param) {
    ESP_LOGI(TAG, "StreamTask bat dau tren Port %d (Core %d)", STREAM_SERVER_PORT, xPortGetCoreID());
    s_stream_server.begin();

    while (true) {
        WiFiClient client = s_stream_server.available();
        if (!client) {
            vTaskDelay(pdMS_TO_TICKS(15));
            continue;
        }

        ESP_LOGI(TAG, "Client ket noi xem Stream tu: %s", client.remoteIP().toString().c_str());

        // Gửi HTTP header cho multipart/x-mixed-replace
        client.print("HTTP/1.1 200 OK\r\n"
                     "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n"
                     "Access-Control-Allow-Origin: *\r\n\r\n");

        while (client.connected()) {
            camera_fb_t *fb = CameraService::getFrame(100);
            if (!fb) {
                vTaskDelay(pdMS_TO_TICKS(10));
                continue;
            }

            char part_hdr[64];
            snprintf(part_hdr, sizeof(part_hdr), "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", (unsigned int)fb->len);
            
            size_t hdr_len = strlen(part_hdr);
            if (client.write((const uint8_t*)part_hdr, hdr_len) != hdr_len) {
                CameraService::returnFrame(fb);
                break;
            }

            if (client.write(fb->buf, fb->len) != fb->len) {
                CameraService::returnFrame(fb);
                break;
            }

            if (client.write((const uint8_t*)"\r\n", 2) != 2) {
                CameraService::returnFrame(fb);
                break;
            }

            CameraService::returnFrame(fb);
            vTaskDelay(pdMS_TO_TICKS(25)); // ~30-40 fps
        }

        client.stop();
        ESP_LOGI(TAG, "Client ngat ket noi stream");
    }
}

bool WebController::init() {
#if ENABLE_MODULE_WEB_CONTROLLER
    WiFi.mode(WIFI_AP_STA);

    bool connected_sta = false;
    String sta_ssid = WIFI_STA_SSID;
    if (sta_ssid.length() > 0 && sta_ssid != "YOUR_WIFI_SSID") {
        ESP_LOGI(TAG, "Dang ket noi WiFi Router: %s ...", sta_ssid.c_str());
        WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASS);
        uint32_t start_time = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - start_time < 7000)) {
            delay(250);
            Serial.print(".");
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
            connected_sta = true;
            s_is_ap_mode = false;
            ESP_LOGI(TAG, "Da ket noi Router WiFi! IP: %s", WiFi.localIP().toString().c_str());
            EventBus::postType(EVENT_WIFI_CONNECTED);
        } else {
            ESP_LOGW(TAG, "Khong the ket noi WiFi Router. Khoi dong Access Point!");
        }
    }

    if (!connected_sta) {
        WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
        s_is_ap_mode = true;
        ESP_LOGI(TAG, "Robot Access Point bat dau: %s (Pass: %s)", WIFI_AP_SSID, WIFI_AP_PASS);
        ESP_LOGI(TAG, "Dia chi IP Access Point: %s", WiFi.softAPIP().toString().c_str());
    }

    // Đăng ký các Endpoint HTTP
    s_http_server.on("/", HTTP_GET, handle_root);
    s_http_server.on("/status", HTTP_GET, handle_status);
    s_http_server.on("/control", HTTP_GET, handle_control);
    s_http_server.on("/capture", HTTP_GET, handle_capture);

    s_http_server.begin();
    ESP_LOGI(TAG, "HTTP Web Server da mo tren Port %d", WEB_SERVER_PORT);

    s_initialized = true;
    return true;
#else
    return false;
#endif
}

bool WebController::startTask(uint8_t core_id) {
#if ENABLE_MODULE_WEB_CONTROLLER
    if (!s_initialized) {
        if (!init()) return false;
    }

    // Khởi chạy Web Task trên Core được chỉ định
    BaseType_t res_web = xTaskCreatePinnedToCore(
        web_task_worker,
        "WebTask",
        4096,
        nullptr,
        1,
        &s_web_task_handle,
        core_id
    );

    // Khởi chạy Stream Task trên Core 0 (Multimedia/Network)
    BaseType_t res_stream = xTaskCreatePinnedToCore(
        stream_task_worker,
        "StreamTask",
        4096,
        nullptr,
        2,
        &s_stream_task_handle,
        CORE_HEAVY_MULTIMEDIA
    );

    return (res_web == pdPASS && res_stream == pdPASS);
#else
    return false;
#endif
}

String WebController::getIP() {
    if (s_is_ap_mode) {
        return WiFi.softAPIP().toString();
    }
    return WiFi.localIP().toString();
}

bool WebController::isAPMode() {
    return s_is_ap_mode;
}
