"""
Robot Xiaozhi - Real-time USB Video Viewer
Cửa sổ hiển thị video trực tiếp từ ESP32-S3 qua cáp USB Serial
"""

import os
import sys
import time
import struct
import cv2
import numpy as np
import serial
import serial.tools.list_ports

# Cấu hình mặc định
DEFAULT_BAUD = 2000000
MAGIC_HEADER = b'\xaa\x55\xaa\x55'
MAGIC_FOOTER = b'\x55\xaa'

def find_esp_port():
    """Tự động dò tìm cổng COM của ESP32"""
    ports = serial.tools.list_ports.comports()
    for port in ports:
        desc = (port.description or "").lower()
        # Ưu tiên chip CH343, CH340, CP210 hoặc cổng COM4
        if "ch343" in desc or "ch340" in desc or "cp210" in desc or "usb-serial" in desc or "com4" in port.device.lower():
            return port.device
    if ports:
        return ports[0].device
    return "COM4"

def main():
    target_port = sys.argv[1] if len(sys.argv) > 1 else find_esp_port()
    print("=" * 60)
    print("       ROBOT XIAOZHI - REAL-TIME VIDEO VIEWER")
    print("=" * 60)
    print(f"[*] Dang mo cong Serial: {target_port} o toc do {DEFAULT_BAUD} baud...")

    try:
        ser = serial.Serial(target_port, DEFAULT_BAUD, timeout=1.0)
    except Exception as e:
        print(f"[!] Khong the mo cong {target_port}: {e}")
        print("[*] Vui long kiem tra lai cap USB hoac tat Serial Monitor neu dang mo.")
        input("Nhan Enter de thoat...")
        return

    # Tao thu muc luu anh chup tren may tinh
    captures_dir = os.path.join(os.path.dirname(__file__), "captures")
    os.makedirs(captures_dir, exist_ok=True)

    window_name = "Robot Xiaozhi - Live Camera (USB Stream)"
    cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)
    cv2.resizeWindow(window_name, 960, 720)

    print("[+] Da ket noi thanh cong! Dang nhan video stream...")
    print("------------------------------------------------------------")
    print(" [S]   : Chup anh luu ve may tinh (thu muc captures/)")
    print(" [C]   : Lenh cho ESP32 chup & luu vao the nho SD")
    print(" [1]   : Che do QVGA (320x240 - sieu muot)")
    print(" [2]   : Che do VGA  (640x480 - tieu chuan)")
    print(" [3]   : Che do SVGA (800x600 - do net cao)")
    print(" [Q/ESC]: Thoat chuong trinh")
    print("------------------------------------------------------------")

    buffer = bytearray()
    fps_time = time.time()
    frame_count = 0
    fps = 0.0
    status_msg = "STREAMING LIVE"
    status_msg_expire = time.time()

    try:
        while True:
            # Doc du lieu tu Serial vao buffer
            waiting = ser.in_waiting
            chunk_size = max(4096, waiting)
            data = ser.read(chunk_size)
            if data:
                buffer.extend(data)

            # Tim goi tin hop le trong buffer
            while True:
                idx = buffer.find(MAGIC_HEADER)
                if idx == -1:
                    # Neu buffer qua lon ma khong thay header, giu lai 4 bytes cuoi
                    if len(buffer) > 4:
                        buffer = buffer[-4:]
                    break

                # Xoa du lieu rac truoc header
                if idx > 0:
                    del buffer[:idx]

                # Kiem tra da du toi thieu 8 bytes (Header 4B + Length 4B) chua
                if len(buffer) < 8:
                    break

                # Doc do dai payload
                payload_len = struct.unpack('<I', buffer[4:8])[0]

                # Bao ve chong gia tri do dai rac
                if payload_len <= 0 or payload_len > 500000:
                    del buffer[:4]  # Bo qua magic byte loi
                    continue

                total_packet_len = 8 + payload_len + 2  # Header + Len + Data + Footer
                if len(buffer) < total_packet_len:
                    break  # Chua du toan bo frame, cho them du lieu

                # Kiem tra footer
                footer = bytes(buffer[8 + payload_len : total_packet_len])
                if footer == MAGIC_FOOTER:
                    jpeg_bytes = bytes(buffer[8 : 8 + payload_len])
                    del buffer[:total_packet_len]

                    # Giai ma JPEG thanh anh OpenCV
                    np_arr = np.frombuffer(jpeg_bytes, dtype=np.uint8)
                    frame = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)

                    if frame is not None:
                        frame_count += 1
                        now = time.time()
                        if now - fps_time >= 1.0:
                            fps = frame_count / (now - fps_time)
                            frame_count = 0
                            fps_time = now

                        # Tao giao dien HUD hien dai tren khung hinh
                        h, w = frame.shape[:2]

                        # Thanh thong tin ben tren (Top Bar)
                        overlay = frame.copy()
                        cv2.rectangle(overlay, (0, 0), (w, 36), (20, 20, 25), -1)
                        # Thanh thong tin ben duoi (Bottom Bar)
                        cv2.rectangle(overlay, (0, h - 30), (w, h), (20, 20, 25), -1)
                        cv2.addWeighted(overlay, 0.65, frame, 0.35, 0, frame)

                        # Top HUD: Ten + FPS + Do phan giai + Dung luong
                        cv2.circle(frame, (16, 18), 6, (0, 0, 255), -1) # Cham do Recording
                        cv2.putText(frame, "LIVE", (28, 22), cv2.FONT_HERSHEY_DUPLEX, 0.45, (255, 255, 255), 1)
                        
                        info_text = f"FPS: {fps:4.1f} | {w}x{h} | {payload_len / 1024:.1f} KB"
                        cv2.putText(frame, info_text, (85, 22), cv2.FONT_HERSHEY_SIMPLEX, 0.45, (0, 255, 180), 1)

                        # Thong bao trang thai
                        if time.time() < status_msg_expire:
                            cv2.putText(frame, status_msg, (w - 240, 22), cv2.FONT_HERSHEY_SIMPLEX, 0.45, (0, 255, 255), 1)

                        # Bottom HUD: Huong dan phim tat
                        tips = "[S] Luu vao PC   [C] Luu the SD   [1-3] Do phan giai   [Q] Thoat"
                        cv2.putText(frame, tips, (10, h - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (200, 200, 200), 1)

                        cv2.imshow(window_name, frame)

                        # Xu ly phim bam
                        key = cv2.waitKey(1) & 0xFF
                        if key == ord('q') or key == 27:  # Q hoac ESC
                            print("\n[*] Dang thoat chuong trinh...")
                            break
                        elif key == ord('s') or key == ord('S'):
                            # Chup anh luu ve PC
                            timestamp = time.strftime("%Y%m%d_%H%M%S")
                            save_path = os.path.join(captures_dir, f"capture_{timestamp}.jpg")
                            with open(save_path, "wb") as f:
                                f.write(jpeg_bytes)
                            status_msg = "DA LUU ANH VAO PC!"
                            status_msg_expire = time.time() + 2.5
                            print(f"[+] Da luu anh vao may tinh: {save_path}")
                        elif key == ord('c') or key == ord('C'):
                            # Gui lenh chup luu vao the SD
                            ser.write(b'c')
                            status_msg = "DANG LUU VAO THE SD..."
                            status_msg_expire = time.time() + 2.5
                            print("[*] Da gui lenh luu anh vao the nho SD tren ESP32!")
                        elif key in (ord('1'), ord('2'), ord('3')):
                            ser.write(bytes([key]))
                            res_names = {'1': 'QVGA (320x240)', '2': 'VGA (640x480)', '3': 'SVGA (800x600)'}
                            status_msg = f"DOI: {res_names[chr(key)]}"
                            status_msg_expire = time.time() + 2.0
                            print(f"[*] Chuyen do phan giai sang: {res_names[chr(key)]}")
                else:
                    # Footer khong khop, bo qua 4 bytes de dong bo lai
                    del buffer[:4]

            # Kiem tra neu nguoi dung bam nut X tren cua so
            if cv2.getWindowProperty(window_name, cv2.WND_PROP_VISIBLE) < 1:
                break

    except KeyboardInterrupt:
        print("\n[*] Nguoi dung dung chuong trinh.")
    finally:
        ser.close()
        cv2.destroyAllWindows()
        print("[*] Da dong cong Serial va thoat.")

if __name__ == "__main__":
    main()
