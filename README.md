# 🤖 Robot Xiaozhi - ESP32-S3-CAM Firmware

Dự án mã nguồn chuẩn hóa dành cho robot AI **Xiaozhi (小智)** chạy trên nền vi điều khiển **ESP32-S3** tích hợp Camera (OV2640 / OV5640) và bộ nhớ mở rộng **PSRAM (Octal SPI - OPI)**.

---

## 📋 Mục lục
- [1. Giới thiệu phần cứng](#1-giới-thiệu-phần-cứng)
- [2. Cấu hình PlatformIO](#2-cấu-hình-platformio)
- [3. Sơ đồ chân kết nối (Pinout)](#3-sơ-đồ-chân-kết-nối-pinout)
- [4. Cấu trúc thư mục](#4-cấu-trúc-thư-mục)
- [5. Hướng dẫn cài đặt và nạp code](#5-hướng-dẫn-cài-đặt-và-nạp-code)
- [6. Xử lý sự cố thường gặp (Troubleshooting)](#6-xử-lý-sự-cố-thường-gặp-troubleshooting)

---

## 1. Giới thiệu phần cứng

| Thành phần | Thông số khuyến nghị |
| :--- | :--- |
| **Vi điều khiển** | ESP32-S3 (Dual-core Xtensa LX7 @ 240MHz) |
| **Flash** | 8MB / 16MB Quad SPI |
| **PSRAM** | 8MB Octal SPI (OPI PSRAM) - *Bắt buộc để xử lý ảnh độ phân giải cao và AI* |
| **Cảm biến Camera** | OV2640 (2MP) hoặc OV5640 (5MP) |
| **Cổng kết nối** | USB Type-C (Native USB CDC Serial) |

---

## 2. Cấu hình PlatformIO

File cấu hình [`platformio.ini`](platformio.ini) đã được tối ưu sẵn:

```ini
[env:esp32-s3-cam]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

monitor_speed = 115200
upload_speed = 921600

; Kích hoạt OPI PSRAM và Flash 16MB
board_build.arduino.memory_type = qio_opi
board_build.flash_mode = qio
board_build.partitions = huge_app.csv
board_upload.flash_size = 16MB

; Build flags cần thiết
build_flags = 
    -D BOARD_HAS_PSRAM
    -D ARDUINO_USB_CDC_ON_BOOT=1
    -D ARDUINO_USB_MODE=1
    -D CORE_DEBUG_LEVEL=3

lib_deps = 
    esp32-camera
```

### Các điểm quan trọng:
- **`BOARD_HAS_PSRAM`**: Bắt buộc để `esp32-camera` cấp phát Frame Buffer trong PSRAM.
- **`ARDUINO_USB_CDC_ON_BOOT=1`**: Cho phép in log `Serial.println()` qua cổng USB Type-C trực tiếp của ESP32-S3.
- **`huge_app.csv`**: Dành hơn 3MB Flash cho phân vùng ứng dụng (App), tránh lỗi tràn bộ nhớ khi tích hợp thêm các thư viện AI/WiFi/WebSockets.

---

## 3. Sơ đồ chân kết nối (Pinout)

### Sơ đồ chân Camera mặc định (`CAMERA_MODEL_ESP32S3_CAM_GENERIC`)

| Tên chân Camera | Chân GPIO ESP32-S3 | Chức năng |
| :--- | :---: | :--- |
| **XCLK** | `GPIO 15` | Master Clock |
| **PCLK** | `GPIO 13` | Pixel Clock |
| **VSYNC** | `GPIO 6` | Vertical Sync |
| **HREF** | `GPIO 7` | Horizontal Reference |
| **SIOD (SDA)** | `GPIO 4` | I2C Data (SCCB) |
| **SIOC (SCL)** | `GPIO 5` | I2C Clock (SCCB) |
| **D0 - D7** | `11, 9, 8, 10, 12, 18, 17, 16` | Data Pins (Y2 - Y9) |
| **PWDN / RESET** | `-1` | Không kết nối / Nối nguồn |

> 💡 **Lưu ý**: Nếu dùng **Seeed Studio XIAO ESP32S3 Sense** hoặc **ESP32-S3-EYE**, hãy mở file [`include/camera_pins.h`](include/camera_pins.h) và bật định nghĩa `#define` tương ứng.

### Sơ đồ chân thẻ nhớ MicroSD (SD_MMC 1-Bit Mode)

| Tín hiệu SD | Chân GPIO ESP32-S3 | Ghi chú |
| :--- | :---: | :--- |
| **CLK** | `GPIO 39` | Clock |
| **CMD** | `GPIO 38` | Command |
| **D0 (DAT0)** | `GPIO 40` | Data Line 0 |

---

## 4. Cấu trúc thư mục

```text
robot_xiaozhi/
├── include/
│   ├── camera_pins.h       # Cấu hình định nghĩa chân camera
│   └── README
├── src/
│   └── main.cpp            # Mã nguồn stream video tốc độ cao qua USB Serial & lưu thẻ MicroSD
├── platformio.ini          # Cấu hình PlatformIO cho ESP32-S3-CAM
├── viewer.py               # Ứng dụng xem video trực tiếp bằng OpenCV trên máy tính
├── run_viewer.bat          # File click mở nhanh Live Video Viewer
├── .gitignore              # Loại trừ build artifacts (.pio, vscode db...)
└── README.md               # Tài liệu hướng dẫn dự án
```

---

## 5. Hướng dẫn cài đặt và nạp code

### 5.1. Yêu cầu môi trường
- Cài đặt **[Visual Studio Code](https://code.visualstudio.com/)**.
- Cài đặt Extension **[PlatformIO IDE](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)** trên VS Code.
- Python 3 với các thư viện: `pip install opencv-python numpy pyserial`.

### 5.2. Các bước thực hiện
1. **Mở dự án**: Mở thư mục `robot_xiaozhi` trong VS Code.
2. **Cắm mạch**: Kết nối ESP32-S3-CAM với máy tính qua cổng Type-C.
3. **Biên dịch (Build)**: Nhấn `Ctrl + Shift + B`.
4. **Nạp Firmware (Upload)**: Chạy task `PlatformIO: Upload`.
5. **Mở Live Video Viewer**:
   - **Cách 1**: Nhấp đúp vào file [`run_viewer.bat`](run_viewer.bat).
   - **Cách 2**: Chạy lệnh trong terminal:
     ```bash
     python viewer.py
     ```
   - **Phím tắt trong cửa sổ Video**:
     - `S`: Chụp ảnh lưu về thư mục `captures/` trên máy tính.
     - `C`: Gửi lệnh cho ESP32 chụp và lưu ảnh vào thẻ nhớ MicroSD.
     - `1` / `2` / `3`: Đổi nhanh độ phân giải (QVGA / VGA / SVGA).
     - `Q` hoặc `ESC`: Thoát viewer.

---

## 6. Xử lý sự cố thường gặp (Troubleshooting)

| Vấn đề | Nguyên nhân | Cách khắc phục |
| :--- | :--- | :--- |
| **`Camera init failed! Ma loi: 0x20002 / 0x105`** | Cáp FPC camera chưa gắn chặt hoặc sai chân GPIO. | Rút cáp camera ra cắm lại cẩn thận; kiểm tra lại cấu hình trong `camera_pins.h`. |
| **`PSRAM: KHONG TIM THAY`** | Module không có PSRAM hoặc cấu hình sai loại PSRAM. | Đảm bảo module là bản `N8R8`/`N16R8`. Kiểm tra dòng `board_build.arduino.memory_type = qio_opi` trong `platformio.ini`. |
| **Không in log ra Serial Monitor** | Cổng Type-C kết nối trực tiếp USB CDC chưa bật. | Đảm bảo cờ `-D ARDUINO_USB_CDC_ON_BOOT=1` có trong `build_flags` của `platformio.ini`. |
| **Lỗi tràn bộ nhớ Flash khi nạp** | Phân vùng mặc định quá nhỏ. | Sử dụng `board_build.partitions = huge_app.csv` đã được cấu hình sẵn. |

---

## 📄 Giấy phép (License)
Dự án được phát hành theo giấy phép mã nguồn mở MIT.
