#pragma once
#include <Arduino.h>

static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>Robot Xiaozhi - Web Controller</title>
  <style>
    :root {
      --bg-primary: #0a0e17;
      --bg-card: rgba(18, 24, 38, 0.85);
      --border-color: rgba(34, 211, 238, 0.2);
      --accent-cyan: #06b6d4;
      --accent-cyan-glow: rgba(6, 182, 212, 0.4);
      --accent-green: #10b981;
      --accent-red: #ef4444;
      --accent-amber: #f59e0b;
      --text-main: #f1f5f9;
      --text-muted: #94a3b8;
    }
    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
      user-select: none;
      -webkit-user-select: none;
    }
    body {
      background-color: var(--bg-primary);
      background-image: radial-gradient(circle at 50% 0%, rgba(6, 182, 212, 0.15), transparent 70%);
      color: var(--text-main);
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 12px;
    }
    header {
      width: 100%;
      max-width: 900px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 12px 18px;
      background: var(--bg-card);
      border: 1px solid var(--border-color);
      border-radius: 14px;
      backdrop-filter: blur(12px);
      margin-bottom: 14px;
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.4);
    }
    .brand {
      display: flex;
      align-items: center;
      gap: 10px;
    }
    .brand-icon {
      font-size: 28px;
    }
    .brand-title {
      font-size: 1.15rem;
      font-weight: 700;
      letter-spacing: 0.5px;
      background: linear-gradient(135deg, #22d3ee, #38bdf8);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
    }
    .status-pill {
      display: flex;
      align-items: center;
      gap: 8px;
      padding: 5px 12px;
      background: rgba(16, 185, 129, 0.15);
      border: 1px solid rgba(16, 185, 129, 0.4);
      border-radius: 20px;
      font-size: 0.85rem;
      color: #34d399;
    }
    .pulse-dot {
      width: 8px;
      height: 8px;
      background-color: var(--accent-green);
      border-radius: 50%;
      box-shadow: 0 0 10px var(--accent-green);
      animation: pulse 1.5s infinite;
    }
    @keyframes pulse {
      0%, 100% { opacity: 1; transform: scale(1); }
      50% { opacity: 0.4; transform: scale(0.85); }
    }
    .main-container {
      width: 100%;
      max-width: 900px;
      display: grid;
      grid-template-columns: 1fr 320px;
      gap: 16px;
    }
    @media (max-width: 768px) {
      .main-container {
        grid-template-columns: 1fr;
      }
    }
    .card {
      background: var(--bg-card);
      border: 1px solid var(--border-color);
      border-radius: 14px;
      padding: 14px;
      backdrop-filter: blur(12px);
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3);
    }
    .card-title {
      font-size: 0.95rem;
      font-weight: 600;
      color: var(--accent-cyan);
      margin-bottom: 12px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      text-transform: uppercase;
      letter-spacing: 0.8px;
    }
    .video-viewport {
      position: relative;
      width: 100%;
      background: #000;
      border-radius: 10px;
      overflow: hidden;
      aspect-ratio: 4/3;
      display: flex;
      align-items: center;
      justify-content: center;
      border: 1px solid rgba(255, 255, 255, 0.1);
    }
    .video-viewport img {
      width: 100%;
      height: 100%;
      object-fit: cover;
      display: block;
    }
    .video-overlay {
      position: absolute;
      top: 10px;
      left: 10px;
      display: flex;
      gap: 6px;
    }
    .badge {
      background: rgba(0, 0, 0, 0.65);
      border: 1px solid rgba(255, 255, 255, 0.2);
      padding: 3px 8px;
      border-radius: 6px;
      font-size: 0.75rem;
      backdrop-filter: blur(4px);
    }
    .badge-live {
      color: #ef4444;
      font-weight: 700;
    }
    .video-actions {
      display: flex;
      gap: 8px;
      margin-top: 12px;
    }
    button, .btn {
      flex: 1;
      padding: 10px 14px;
      border-radius: 8px;
      border: 1px solid var(--border-color);
      background: rgba(6, 182, 212, 0.15);
      color: var(--text-main);
      font-weight: 600;
      font-size: 0.85rem;
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 6px;
      transition: all 0.15s ease;
      touch-action: manipulation;
    }
    button:active, .btn:active {
      transform: scale(0.96);
      background: var(--accent-cyan);
      color: #000;
    }
    .btn-secondary {
      background: rgba(255, 255, 255, 0.05);
      border-color: rgba(255, 255, 255, 0.1);
    }
    .btn-danger {
      background: rgba(239, 68, 68, 0.2);
      border-color: rgba(239, 68, 68, 0.4);
      color: #fca5a5;
    }
    .btn-danger:active {
      background: var(--accent-red);
      color: #fff;
    }
    .telemetry-grid {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 8px;
      margin-top: 12px;
    }
    .telemetry-item {
      background: rgba(0, 0, 0, 0.35);
      border: 1px solid rgba(255, 255, 255, 0.06);
      padding: 8px 10px;
      border-radius: 8px;
    }
    .tele-label {
      font-size: 0.72rem;
      color: var(--text-muted);
      text-transform: uppercase;
    }
    .tele-val {
      font-size: 0.95rem;
      font-weight: 700;
      color: #38bdf8;
      margin-top: 2px;
    }
    .dpad-container {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      grid-template-rows: repeat(3, 1fr);
      gap: 10px;
      width: 220px;
      height: 220px;
      margin: 10px auto;
    }
    .dpad-btn {
      border-radius: 14px;
      font-size: 1.5rem;
      display: flex;
      align-items: center;
      justify-content: center;
      background: rgba(255, 255, 255, 0.06);
      border: 1px solid rgba(255, 255, 255, 0.15);
      box-shadow: 0 4px 10px rgba(0, 0, 0, 0.3);
      cursor: pointer;
      user-select: none;
      -webkit-user-select: none;
      touch-action: none;
    }
    .dpad-btn:active, .dpad-btn.pressed {
      background: var(--accent-cyan);
      color: #000;
      box-shadow: 0 0 15px var(--accent-cyan);
      transform: scale(0.92);
    }
    .dpad-up    { grid-column: 2; grid-row: 1; }
    .dpad-left  { grid-column: 1; grid-row: 2; }
    .dpad-stop  { grid-column: 2; grid-row: 2; background: rgba(239, 68, 68, 0.25); border-color: rgba(239, 68, 68, 0.5); color: #f87171; }
    .dpad-right { grid-column: 3; grid-row: 2; }
    .dpad-down  { grid-column: 2; grid-row: 3; }

    .slider-row {
      margin: 14px 0 8px 0;
    }
    .slider-label {
      display: flex;
      justify-content: space-between;
      font-size: 0.8rem;
      color: var(--text-muted);
      margin-bottom: 6px;
    }
    input[type=range] {
      width: 100%;
      height: 6px;
      border-radius: 5px;
      background: rgba(255, 255, 255, 0.15);
      outline: none;
      accent-color: var(--accent-cyan);
    }
    .emotions-grid {
      display: grid;
      grid-template-columns: repeat(5, 1fr);
      gap: 6px;
      margin-top: 10px;
    }
    .emotion-btn {
      padding: 10px 0;
      font-size: 1.3rem;
      background: rgba(255, 255, 255, 0.05);
      border-radius: 8px;
      border: 1px solid rgba(255, 255, 255, 0.08);
    }
    .log-box {
      margin-top: 12px;
      background: #000;
      border-radius: 8px;
      padding: 8px 10px;
      font-family: monospace;
      font-size: 0.75rem;
      color: #34d399;
      height: 50px;
      overflow-y: auto;
      border: 1px solid rgba(255, 255, 255, 0.08);
    }
  </style>
</head>
<body>

  <header>
    <div class="brand">
      <div class="brand-icon">🤖</div>
      <div>
        <div class="brand-title">ROBOT XIAOZHI</div>
        <div style="font-size: 0.72rem; color: var(--text-muted);">ESP32-S3 Web Controller & Video Streamer</div>
      </div>
    </div>
    <div class="status-pill">
      <div class="pulse-dot"></div>
      <span id="connStatus">ONLINE</span>
    </div>
  </header>

  <div class="main-container">
    <!-- CỘT TRÁI: VIDEO STREAM -->
    <div class="card">
      <div class="card-title">
        <span>Khung hình Camera trực tiếp</span>
        <span id="fpsCounter" style="font-size: 0.78rem; color: var(--text-muted);">30 FPS</span>
      </div>

      <div class="video-viewport" id="viewport">
        <img id="streamImg" alt="Camera Stream" src="" />
        <div class="video-overlay">
          <div class="badge badge-live">● LIVE</div>
          <div class="badge" id="resBadge">VGA (640x480)</div>
        </div>
      </div>

      <div class="video-actions">
        <button id="btnCapture" onclick="takeSnapshot()">📷 Chụp & Lưu SD</button>
        <button class="btn-secondary" onclick="toggleStreamMode()" id="btnMode">⚡ Stream Port 81</button>
        <button class="btn-secondary" onclick="toggleLamp()" id="btnLamp">💡 Đèn Trợ Sáng</button>
      </div>

      <!-- Telemetry Status -->
      <div class="telemetry-grid">
        <div class="telemetry-item">
          <div class="tele-label">Địa chỉ IP Robot</div>
          <div class="tele-val" id="teleIp">...</div>
        </div>
        <div class="telemetry-item">
          <div class="tele-label">Sóng WiFi (RSSI)</div>
          <div class="tele-val" id="teleRssi">...</div>
        </div>
        <div class="telemetry-item">
          <div class="tele-label">Free SRAM</div>
          <div class="tele-val" id="teleHeap">...</div>
        </div>
        <div class="telemetry-item">
          <div class="tele-label">Free PSRAM</div>
          <div class="tele-val" id="telePsram">...</div>
        </div>
      </div>

      <div class="log-box" id="logBox">> Hệ thống sẵn sàng kết nối...</div>
    </div>

    <!-- CỘT PHẢI: BỘ ĐIỀU KHIỂN ROBOT -->
    <div class="card">
      <div class="card-title">
        <span>Điều khiển Di chuyển</span>
        <span style="font-size: 0.75rem; color: var(--text-muted);">W-A-S-D / Cảm ứng</span>
      </div>

      <!-- D-Pad Buttons -->
      <div class="dpad-container">
        <button class="dpad-btn dpad-up" id="btnForward">⬆</button>
        <button class="dpad-btn dpad-left" id="btnLeft">⬅</button>
        <button class="dpad-btn dpad-stop" id="btnStop">⏹</button>
        <button class="dpad-btn dpad-right" id="btnRight">➡</button>
        <button class="dpad-btn dpad-down" id="btnBackward">⬇</button>
      </div>

      <!-- Speed Slider -->
      <div class="slider-row">
        <div class="slider-label">
          <span>Tốc độ di chuyển (PWM)</span>
          <span id="speedVal" style="color: var(--accent-cyan); font-weight: bold;">200</span>
        </div>
        <input type="range" id="speedSlider" min="50" max="255" value="200" oninput="updateSpeed(this.value)">
      </div>

      <!-- Biểu cảm khuôn mặt -->
      <div style="margin-top: 14px;">
        <div class="card-title" style="margin-bottom: 4px;">
          <span>Biểu cảm Khuôn mặt</span>
        </div>
        <div class="emotions-grid">
          <button class="emotion-btn" onclick="sendEmotion('happy')" title="Vui vẻ">😊</button>
          <button class="emotion-btn" onclick="sendEmotion('sad')" title="Buồn">😢</button>
          <button class="emotion-btn" onclick="sendEmotion('surprised')" title="Ngạc nhiên">😲</button>
          <button class="emotion-btn" onclick="sendEmotion('angry')" title="Tức giận">😠</button>
          <button class="emotion-btn" onclick="sendEmotion('normal')" title="Bình thường">😐</button>
        </div>
      </div>
    </div>
  </div>

  <script>
    let currentSpeed = 200;
    let streamMode = 'port81'; // 'port81' or 'snapshot'
    let lampState = false;
    let pollInterval = null;

    function log(msg) {
      const box = document.getElementById('logBox');
      const time = new Date().toLocaleTimeString();
      box.innerHTML = `[${time}] ${msg}<br>` + box.innerHTML;
    }

    function startStream() {
      const img = document.getElementById('streamImg');
      const host = window.location.hostname || '192.168.4.1';
      if (streamMode === 'port81') {
        img.src = `http://${host}:81/stream`;
        img.onerror = () => {
          log("Không kết nối được Port 81 stream, chuyển sang chế độ Snapshot polling...");
          toggleStreamMode();
        };
        log(`Đang stream video từ http://${host}:81/stream`);
      } else {
        if (pollInterval) clearInterval(pollInterval);
        pollInterval = setInterval(() => {
          img.src = `/capture?t=${Date.now()}`;
        }, 150);
        log("Đang chạy chế độ Snapshot Polling");
      }
    }

    function toggleStreamMode() {
      const img = document.getElementById('streamImg');
      const btn = document.getElementById('btnMode');
      if (pollInterval) {
        clearInterval(pollInterval);
        pollInterval = null;
      }
      if (streamMode === 'port81') {
        streamMode = 'snapshot';
        btn.innerText = "⚡ Stream Snapshot";
      } else {
        streamMode = 'port81';
        btn.innerText = "⚡ Stream Port 81";
      }
      startStream();
    }

    function sendCommand(cmd, params = {}) {
      let query = `cmd=${cmd}`;
      for (const key in params) {
        query += `&${key}=${encodeURIComponent(params[key])}`;
      }
      fetch(`/control?${query}`)
        .then(res => res.text())
        .then(txt => log(`Lệnh: ${cmd} -> ${txt}`))
        .catch(err => log(`Lỗi gửi lệnh ${cmd}`));
    }

    function updateSpeed(val) {
      currentSpeed = val;
      document.getElementById('speedVal').innerText = val;
    }

    function sendEmotion(emo) {
      sendCommand('emotion', { val: emo });
    }

    function takeSnapshot() {
      sendCommand('capture');
      log("Yêu cầu chụp ảnh lưu thẻ nhớ MicroSD!");
    }

    function toggleLamp() {
      lampState = !lampState;
      sendCommand('lamp', { val: lampState ? 'on' : 'off' });
      document.getElementById('btnLamp').innerText = lampState ? "💡 Tắt Đèn" : "💡 Bật Đèn";
    }

    // Gắn sự kiện cho các nút D-Pad (Giữ để chạy, nhả để dừng)
    function setupHoldButton(btnId, cmd) {
      const btn = document.getElementById(btnId);
      const startAction = (e) => {
        e.preventDefault();
        btn.classList.add('pressed');
        sendCommand(cmd, { speed: currentSpeed });
      };
      const stopAction = (e) => {
        e.preventDefault();
        btn.classList.remove('pressed');
        sendCommand('stop');
      };

      btn.addEventListener('mousedown', startAction);
      btn.addEventListener('mouseup', stopAction);
      btn.addEventListener('mouseleave', stopAction);
      btn.addEventListener('touchstart', startAction, { passive: false });
      btn.addEventListener('touchend', stopAction, { passive: false });
    }

    setupHoldButton('btnForward', 'forward');
    setupHoldButton('btnBackward', 'backward');
    setupHoldButton('btnLeft', 'left');
    setupHoldButton('btnRight', 'right');
    document.getElementById('btnStop').addEventListener('click', () => sendCommand('stop'));

    // Điều khiển bằng bàn phím máy tính
    const keyMap = {
      'KeyW': 'forward', 'ArrowUp': 'forward',
      'KeyS': 'backward', 'ArrowDown': 'backward',
      'KeyA': 'left', 'ArrowLeft': 'left',
      'KeyD': 'right', 'ArrowRight': 'right',
      'Space': 'stop'
    };
    let activeKey = null;

    window.addEventListener('keydown', (e) => {
      if (keyMap[e.code] && activeKey !== e.code) {
        activeKey = e.code;
        const cmd = keyMap[e.code];
        sendCommand(cmd, { speed: currentSpeed });
      }
    });

    window.addEventListener('keyup', (e) => {
      if (activeKey === e.code) {
        activeKey = null;
        sendCommand('stop');
      }
    });

    // Cập nhật trạng thái hệ thống định kỳ (Telemetry)
    function fetchStatus() {
      fetch('/status')
        .then(r => r.json())
        .then(data => {
          if (data.ip) document.getElementById('teleIp').innerText = data.ip;
          if (data.rssi !== undefined) document.getElementById('teleRssi').innerText = data.rssi + " dBm";
          if (data.heap) document.getElementById('teleHeap').innerText = Math.round(data.heap / 1024) + " KB";
          if (data.psram) document.getElementById('telePsram').innerText = Math.round(data.psram / 1024) + " KB";
        })
        .catch(() => {});
    }

    setInterval(fetchStatus, 3000);
    fetchStatus();
    startStream();
  </script>
</body>
</html>
)rawliteral";
