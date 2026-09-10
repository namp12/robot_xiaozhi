@echo off
title Robot Xiaozhi - Real-time Video Viewer
echo ===================================================
echo     Khoi dong Robot Xiaozhi - Live Video Viewer
echo ===================================================
python viewer.py
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Thu chay bang moi truong PlatformIO Python...
    "%USERPROFILE%\.platformio\penv\Scripts\python.exe" viewer.py
)
pause
