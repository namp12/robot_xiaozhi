# 🚗 Component `MotorDriver` - Điều Khiển Động Cơ Bánh Xe

Component `MotorDriver` cung cấp giao diện điều khiển chuyển động cho robot sử dụng hệ dẫn động vi sai 2 bánh (Differential Drive) với 2 động cơ giảm tốc kim loại **N20** và mạch cầu H (như **L298N Mini**, **DRV8833**, hoặc **MX1508**).

---

## 📂 Các file trong component
* [`MotorDriver.h`](MotorDriver.h): Khai báo các hàm điều khiển cơ bản: `forward()`, `backward()`, `turnLeft()`, `turnRight()`, `stop()`.
* [`MotorDriver.cpp`](MotorDriver.cpp): Hiện thực chi tiết xuất tín hiệu logic GPIO ra các chân điều khiển cầu H.

---

## 🔌 Sơ đồ chân kết nối mặc định (Cấu hình tại `app_config.h`)

| Tín hiệu Cầu H | Chân GPIO ESP32-S3 | Chức năng |
| :--- | :---: | :--- |
| **IN1 (Motor Trái)** | `GPIO 2` | Chân điều khiển cực 1 động cơ trái |
| **IN2 (Motor Trái)** | `GPIO 1` | Chân điều khiển cực 2 động cơ trái |
| **IN3 (Motor Phải)** | `GPIO 47` | Chân điều khiển cực 1 động cơ phải |
| **IN4 (Motor Phải)** | `GPIO 48` | Chân điều khiển cực 2 động cơ phải |

---

## 📊 Bảng logic điều khiển cầu H

| Chuyển động | IN1 (Trái) | IN2 (Trái) | IN3 (Phải) | IN4 (Phải) | Trạng thái xe |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **`forward()`** | **HIGH** | LOW | **HIGH** | LOW | Cả 2 bánh quay tiến |
| **`backward()`** | LOW | **HIGH** | LOW | **HIGH** | Cả 2 bánh quay lùi |
| **`turnLeft()`** | LOW | **HIGH** | **HIGH** | LOW | Bánh trái lùi, bánh phải tiến -> Xoay tròn tại chỗ sang trái |
| **`turnRight()`** | **HIGH** | LOW | LOW | **HIGH** | Bánh trái tiến, bánh phải lùi -> Xoay tròn tại chỗ sang phải |
| **`stop()`** | LOW | LOW | LOW | LOW | Dừng hẳn 2 động cơ (thả trôi/phanh) |

---

## ⚠️ Lưu ý kỹ thuật quan trọng về nguồn điện
* Động cơ N20 khi bắt đầu quay hoặc bị kẹt có thể kéo dòng từ **200mA - 800mA**, rất dễ làm sụt áp nguồn của ESP32-S3 gây ra hiện tượng **Reset chip liên tục (Brownout Detector reset)**.
* **Bắt buộc**: Cần hàn một tụ hóa có điện dung từ **470uF đến 1000uF (16V - 25V)** song song với đường nguồn cấp cho mạch cầu H và mạch ESP32 như đã thể hiện trên sơ đồ nguyên lý của robot.
