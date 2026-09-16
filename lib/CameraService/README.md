# 📷 Component `CameraService` - Quản Lý Camera & Stream Video Serial

Component `CameraService` đảm nhiệm toàn bộ tác vụ liên quan đến cảm biến máy ảnh (OV2640 / OV5640) trên ESP32-S3: khởi tạo phần cứng, quản lý Frame Buffer trong bộ nhớ mở rộng **OPI PSRAM**, lưu ảnh chụp vào thẻ nhớ MicroSD, và truyền luồng video thời gian thực qua cổng USB Serial tốc độ cao (2.000.000 baud).

---

## 📂 Các file trong component
* [`CameraService.h`](CameraService.h): Khai báo các hàm khởi tạo, chạy Task độc lập và yêu cầu chụp ảnh.
* [`CameraService.cpp`](CameraService.cpp): Hiện thực chi tiết Task worker chạy trên **Core 0**, đóng gói khung hình JPEG và giao thức giao tiếp với máy tính.

---

## ⚙️ Các thông số kỹ thuật cốt lõi

* **Nhân thực thi (Core)**: **Core 0** (`CORE_HEAVY_MULTIMEDIA`), giúp việc đọc camera và truyền dữ liệu lớn không làm nghẽn logic điều khiển trên Core 1.
* **Cơ chế bộ nhớ**:
  * **Khi có PSRAM**: Cấp phát 2 Frame Buffer (`fb_count = 2`) dạng Double Buffering trực tiếp trong **OPI PSRAM**, độ phân giải mặc định **VGA (640x480)**, chất lượng JPEG 12.
  * **Khi không có PSRAM**: Tự động hạ xuống **QVGA (320x240)** lưu trong DRAM nội để tránh crash chip.
* **Giao thức đóng gói Video qua Serial**:
  Mỗi khung hình JPEG được đóng gói theo định dạng binary chuẩn:
  ```text
  [MAGIC HEADER: 4 Bytes] [ĐỘ DÀI FRAME: 4 Bytes] [DỮ LIỆU JPEG BUFFER] [MAGIC FOOTER: 2 Bytes]
  0xAA, 0x55, 0xAA, 0x55       uint32_t (Little Endian)      ...                0x55, 0xAA
  ```
  *(Định dạng này được thiết kế để ứng dụng `viewer.py` trên máy tính bắt đồng bộ khung hình siêu chính xác và không bị rách hình).*

---

## ⌨️ Các lệnh điều khiển từ xa qua Serial (từ PC)

Trong khi Camera đang stream, người dùng từ máy tính có thể gửi các ký tự sau qua cổng Serial:
* `'1'`: Chuyển độ phân giải sang **QVGA (320x240)** - FPS cao.
* `'2'`: Chuyển độ phân giải sang **VGA (640x480)** - Cân bằng độ nét và tốc độ.
* `'3'`: Chuyển độ phân giải sang **SVGA (800x600)** - Độ nét cao.
* `'c'` hoặc `'s'`: Kích hoạt chụp 1 bức ảnh và lưu vào thẻ nhớ MicroSD với tên `/photo_XXXX.jpg`.

---

## 🚀 Cách sử dụng trong code
```cpp
#include <CameraService.h>

void setup() {
    // 1. Khởi tạo phần cứng camera
    if (CameraService::init()) {
        // 2. Chạy Task camera độc lập trên Core 0
        CameraService::startTask(0);
    }
}

// Bất cứ khi nào muốn chụp ảnh lưu thẻ nhớ:
CameraService::requestCapture();
```
