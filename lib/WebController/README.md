# WebController - Bộ điều khiển Web & Video Streamer qua WiFi cho Robot Xiaozhi

> **Bản quyền & Quyền sở hữu (c) 2026 Phương Nam (phuongnam). Toàn quyền bảo lưu.**  
> *Nghiêm cấm sao chép, sử dụng hoặc phân phối lại dưới mọi hình thức khi chưa có sự đồng ý bằng văn bản của tác giả.*

---

## 📌 Giới thiệu tổng quan
`WebController` là dịch vụ mạng không dây tích hợp sẵn trên ESP32-S3, cho phép người dùng điều khiển Robot Xiaozhi từ xa và xem trực tiếp hình ảnh camera từ bất kỳ trình duyệt web nào (Điện thoại, Máy tính bảng, Laptop) mà **không cần cài đặt app**.

---

## 🌟 Tính năng nổi bật
1. **Truyền hình ảnh thời gian thực (MJPEG Live Stream)**:
   - Stream video trực tiếp tốc độ cao (25-30 FPS) qua cổng riêng biệt `:81/stream`.
   - Hỗ trợ chế độ Snapshot Polling dự phòng qua cổng `:80/capture`.
2. **Giao diện Cyberpunk Dark Mode**:
   - Thiết kế tối ưu cho cả cảm ứng trên điện thoại và bàn phím máy tính.
   - Hiển thị HUD thời gian thực: Địa chỉ IP, Cường độ sóng WiFi RSSI, RAM khả dụng, PSRAM khả dụng, Uptime.
3. **Bộ phím điều khiển chuyển động đa năng (D-Pad)**:
   - Các nút Tiến, Lùi, Trái, Phải, Dừng.
   - **Tính năng giữ để chạy - nhả để dừng (Hold to move, Release to stop)** an toàn tuyệt đối.
   - Hỗ trợ phím tắt bàn phím PC: `W`, `A`, `S`, `D` hoặc các phím mũi tên `↑`, `↓`, `←`, `→`, phím `Space` để dừng khẩn cấp.
4. **Thanh trượt chỉnh tốc độ (Speed Slider)**:
   - Tùy chỉnh tốc độ động cơ PWM linh hoạt từ 50 đến 255.
5. **Nút chụp ảnh lưu thẻ nhớ SD (Snapshot to SD)**:
   - Nhấn chụp ngay trên trình duyệt, ảnh lập tức được lưu vào thẻ nhớ MicroSD trên robot.
6. **Bảng biểu cảm khuôn mặt**:
   - Gửi lệnh đổi biểu cảm mắt robot: Vui vẻ (Happy), Buồn (Sad), Ngạc nhiên (Surprised), Tức giận (Angry), Bình thường (Normal).
7. **Đèn trợ sáng ban đêm (Lamp LED)**:
   - Bật / Tắt đèn LED flash chỉ với 1 chạm.

---

## 📶 Chế độ mạng WiFi
- **Access Point (AP Mode - Mặc định)**:
  - Tên WiFi (SSID): `Robot-Xiaozhi`
  - Mật khẩu: `12345678`
  - Địa chỉ IP mặc định: `http://192.168.4.1`
- **Station Mode (STA Mode)**:
  - Tự động kết nối vào Router WiFi gia đình nếu được cấu hình trong `include/app_config.h`.
  - Hiển thị địa chỉ IP cục bộ được cấp qua Serial Monitor.

---

## 🛠 Danh sách API Endpoint
| Endpoint | Phương thức | Mô tả |
| :--- | :--- | :--- |
| `/` | `GET` | Giao diện điều khiển Web Dashboard |
| `/status` | `GET` | Trả về JSON trạng thái (IP, RSSI, Heap, PSRAM, Uptime) |
| `/control?cmd=forward&speed=200` | `GET` | Lệnh robot tiến lên |
| `/control?cmd=backward&speed=200` | `GET` | Lệnh robot lùi lại |
| `/control?cmd=left&speed=180` | `GET` | Lệnh robot rẽ trái |
| `/control?cmd=right&speed=180` | `GET` | Lệnh robot rẽ phải |
| `/control?cmd=stop` | `GET` | Dừng robot ngay lập tức |
| `/control?cmd=capture` | `GET` | Chụp ảnh lưu vào MicroSD |
| `/control?cmd=emotion&val=happy` | `GET` | Đổi biểu cảm khuôn mặt |
| `/control?cmd=lamp&val=on` | `GET` | Bật/tắt đèn flash trợ sáng |
| `/capture` | `GET` | Lấy 1 frame ảnh JPEG duy nhất |
| `:81/stream` | `GET` | MJPEG video stream liên tục |
