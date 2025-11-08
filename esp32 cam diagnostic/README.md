# ESP32-CAM Complete Diagnostic Test

A comprehensive diagnostic tool specifically designed for ESP32-CAM modules. Tests camera functionality, WiFi capabilities, SD card storage, and all peripheral systems.

## Features

This diagnostic performs 12 specialized tests for ESP32-CAM:

1. System Information - Chip details, memory status, PSRAM availability
2. Camera Test - OV2640 sensor initialization and capture testing
3. SD Card Test - MMC interface testing and file operations
4. GPIO Test - Available pin functionality testing
5. Flash LED Test - Built-in LED control and PWM brightness
6. I2C Test - Camera sensor communication verification
7. SPI Test - External device interface testing
8. WiFi Test - Network scanning and access point mode
9. Memory Test - Heap and PSRAM allocation testing
10. Temperature Test - CPU temperature monitoring
11. Streaming Test - Continuous camera capture performance
12. Web Server Test - Live camera streaming via HTTP

## Hardware Requirements

### ESP32-CAM Module
- AI-Thinker ESP32-CAM (most common variant)
- OV2640 camera sensor
- Built-in flash LED
- MicroSD card slot

### Optional Components
- MicroSD card (for image storage testing)
- External antenna (for improved WiFi range)
- USB-to-Serial adapter for programming

### Power Requirements
- 5V power supply (camera requires stable power)
- Minimum 2A current capability recommended

## Pin Configuration (AI-Thinker ESP32-CAM)

### Camera Pins
```
Y9  - GPIO35    Y8  - GPIO34    Y7  - GPIO39    Y6  - GPIO36
Y5  - GPIO21    Y4  - GPIO19    Y3  - GPIO18    Y2  - GPIO5
VSYNC - GPIO25  HREF - GPIO23  PCLK - GPIO22   XCLK - GPIO0
SIOD - GPIO26   SIOC - GPIO27   RESET - N/A     PWDN - GPIO32
```

### Additional Pins
```
GPIO4  - Flash LED (built-in)
GPIO12 - Available for external use
GPIO13 - Available for external use  
GPIO15 - Available for external use
GPIO16 - Available for external use (UART2 RX)
```

### SD Card Interface
```
Uses MMC interface (no CS pin required)
CMD  - GPIO15   CLK  - GPIO14   D0   - GPIO2
D1   - GPIO4    D2   - GPIO12   D3   - GPIO13
```

## Installation

### Arduino IDE Setup
1. Install ESP32 board package in Arduino IDE
2. Select "AI Thinker ESP32-CAM" from Tools > Board
3. Set partition scheme to "Huge APP (3MB No OTA/1MB SPIFFS)"
4. Install required libraries:
   - ESP32 Camera library (built-in)
   - WiFi library (built-in)
   - SD_MMC library (built-in)

### Programming Setup
1. Connect ESP32-CAM to USB-to-Serial adapter:
   - VCC to 5V
   - GND to GND  
   - U0R to TX
   - U0T to RX
   - GPIO0 to GND (for programming mode)
2. Upload the diagnostic code
3. Remove GPIO0 connection and reset module

## Usage

### Basic Testing
1. Upload ESP32_CAM_Complete_Test.ino to your module
2. Open Serial Monitor at 115200 baud rate
3. Reset the ESP32-CAM module
4. Review comprehensive test results
5. Monitor live camera and system data

### Web Interface Testing
1. If camera and WiFi tests pass, web server starts automatically
2. Connect to WiFi network "ESP32-CAM-Test" (password: 12345678)
3. Open browser to module IP address (shown in serial output)
4. Access live camera stream at /stream
5. Capture photos at /capture
6. View system status at /status

## Expected Output

```
==========================================
      ESP32-CAM COMPLETE DIAGNOSTIC      
==========================================

1. SYSTEM INFORMATION
----------------------
Chip Model: ESP32-D0WDQ6
CPU Frequency: 240 MHz
PSRAM: Available (Good for camera)
Status: PASS

2. CAMERA TEST (OV2640)
------------------------
PSRAM found - Using high quality settings
Initializing camera... SUCCESS
Camera sensor ID: 0x26
Capturing test image... SUCCESS (45234 bytes, 1600x1200)
Status: PASS

...

==========================================
         ESP32-CAM TEST SUMMARY          
==========================================
OVERALL RESULT: 12/12 tests passed (100.0%)
EXCELLENT! ESP32-CAM is fully functional
```

## Troubleshooting

### Camera Issues
- Camera initialization failed: Check power supply (needs stable 5V)
- No PSRAM detected: Some modules lack PSRAM, reduces image quality
- Capture fails: Verify all camera pin connections
- Poor image quality: Check lens focus and lighting

### Power Issues
- Brown-out detector triggered: Increase power supply current capacity
- Random resets: Use quality 5V supply with proper filtering
- WiFi connection drops: Power supply may be insufficient

### SD Card Issues
- SD card not detected: Ensure card is properly inserted and formatted (FAT32)
- File operations fail: Check SD card compatibility (Class 10 recommended)
- Slow performance: Use high-speed SD cards for better video recording

### Programming Issues
- Upload fails: Ensure GPIO0 is connected to GND during programming
- Module not responding: Check serial connections and baud rate
- Compilation errors: Verify correct board selection and library installation

## Advanced Features

### Camera Configuration
The diagnostic tests multiple camera settings:
- Frame sizes from QVGA (320x240) to UXGA (1600x1200)
- JPEG quality levels (10-63, lower = better quality)
- Frame buffer management (single/double buffering)

### Memory Management
- Automatic PSRAM detection and utilization
- Heap vs PSRAM allocation testing
- Memory leak detection during continuous operation

### Web Streaming
- MJPEG streaming for live video
- Single photo capture with download
- Real-time system status monitoring
- Multi-client support testing

## Performance Optimization

### For Best Camera Performance
- Use modules with PSRAM (4MB recommended)
- Ensure stable 5V power supply
- Use high-speed SD cards (Class 10 or better)
- Optimize frame size based on application needs

### For Reliable Operation
- Monitor CPU temperature (keep below 70°C)
- Use proper cooling if in enclosed spaces
- Regular memory usage monitoring
- Implement watchdog timers for production use

## Technical Specifications

### Tested Configurations
- Frame rates: Up to 25 FPS at VGA resolution
- Image formats: JPEG, RGB565, YUV422
- Maximum resolution: 1600x1200 (UXGA)
- Streaming: MJPEG over HTTP
- Storage: MicroSD up to 32GB (FAT32)

### Memory Usage
- Minimum heap required: 100KB free
- PSRAM usage: 2-4MB for high-quality images
- SD card buffer: 512 bytes to 64KB configurable
- Web server overhead: ~50KB heap

## Version

Version 1.0 - Complete ESP32-CAM hardware diagnostic suite with web interface