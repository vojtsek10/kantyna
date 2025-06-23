@echo off
echo ===============================
echo ESP32 Cafeteria Setup Script
echo ===============================
echo.

echo Checking if config.h exists...
if exist config.h (
    echo config.h already exists. Skipping template copy.
) else (
    echo Copying config_template.h to config.h...
    copy config_template.h config.h
    echo.
    echo IMPORTANT: Please edit config.h with your WiFi and Firebase credentials!
    echo.
)

echo Setup complete!
echo.
echo Next steps:
echo 1. Edit config.h with your credentials
echo 2. Install required Arduino libraries (see README.md)
echo 3. Upload kantyna.ino to your ESP32
echo.
pause