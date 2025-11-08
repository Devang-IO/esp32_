# ESP32 Diagnostic Test

A comprehensive hardware diagnostic tool for ESP32 development boards that tests all major peripherals and system functions.

## Features

This diagnostic code performs 12 comprehensive tests:

1. System Information - Chip model, CPU frequency, memory status
2. GPIO - LED control and button input testing
3. ADC - Analog input reading with multiple samples
4. DAC - Analog output generation on both channels
5. PWM - Pulse width modulation with fade effects
6. I2C - Bus scanning for connected devices
7. SPI - Communication loopback testing
8. UART - Serial communication verification
9. WiFi - Network scanning and access point mode
10. Timer - Timing accuracy verification
11. Memory - Allocation and pattern testing
12. Temperature - CPU temperature monitoring

## Hardware Requirements

### Minimum Setup
- ESP32 Development Board
- Built-in LED (GPIO2)
- Built-in BOOT button (GPIO0)

### Optional Components
- External LED on GPIO23
- Potentiometer on GPIO34 for ADC testing
- I2C devices for bus scanning
- SPI devices for communication testing

## Pin Configuration

```
GPIO2  - Built-in LED
GPIO23 - External LED (optional)
GPIO0  - BOOT button
GPIO34 - ADC input
GPIO25 - DAC1 output
GPIO26 - DAC2 output
GPIO18 - PWM output
GPIO21 - I2C SDA
GPIO22 - I2C SCL
GPIO16 - UART2 RX
GPIO17 - UART2 TX
```

## Installation

1. Open Arduino IDE
2. Select ESP32 board (Tools > Board > ESP32 Arduino > ESP32 Dev Module)
3. Select correct COM port (Tools > Port)
4. Open ESP32_Complete_Test.ino
5. Upload to your ESP32

## Usage

1. Upload the code to your ESP32
2. Open Serial Monitor at 115200 baud rate
3. Reset the ESP32 to start the diagnostic
4. Review the comprehensive test results
5. Monitor live system data in continuous loop

## Output Example

```
========================================
     ESP32 COMPLETE HARDWARE TEST      
========================================

1. SYSTEM INFORMATION
----------------------
Chip Model: ESP32-D0WDQ6
CPU Frequency: 240 MHz
Flash Size: 4 MB
Free Heap: 294524 bytes
Status: PASS

...

========================================
           TEST RESULTS SUMMARY         
========================================
1.  System Info:    PASS
2.  GPIO:           PASS
...
OVERALL RESULT: 12/12 tests passed (100.0%)
EXCELLENT! All tests passed!
```

## Troubleshooting

### Common Issues
- WiFi test shows no networks: Normal if no WiFi networks available
- I2C shows no devices: Expected if no I2C devices connected
- SPI loopback fails: Connect MISO to MOSI for full loopback test
- UART shows empty receive: Connect RX to TX on pins 16-17 for echo test

### Compilation Errors
- Ensure ESP32 board package is installed
- Select correct ESP32 board variant
- Use partition scheme with sufficient app space
- Check that all required libraries are available

### Memory Issues
- Use "Default 4MB with spiffs" partition scheme
- Or select "Huge APP (3MB No OTA)" for maximum program space
- Ensure ESP32 has sufficient flash memory

## Technical Details

### Memory Usage
- Optimized for minimal RAM usage with bit-packed result structure
- Efficient string handling to reduce flash usage
- Careful memory management in test functions

### Test Methodology
- Each test includes proper initialization and cleanup
- Multiple samples taken for ADC accuracy
- Timing verification for critical functions
- Memory allocation testing with pattern verification

### Continuous Monitoring
- Live system status updates every 5 seconds
- Real-time heap memory monitoring
- CPU temperature tracking
- ADC value monitoring
- Visual LED activity indicator

## Customization

### Modifying Pin Assignments
Edit the pin definitions at the top of the code:

```cpp
#define LED_PIN 2          // Change built-in LED pin
#define ADC_PIN 34         // Change ADC input pin
#define PWM_PIN 18         // Change PWM output pin
```

### Adding Custom Tests
Add new test functions following the existing pattern:

```cpp
void testCustom() {
  Serial.println("\nCUSTOM TEST");
  // Your test code here
  results.custom = 1;  // Add to results structure
  Serial.println("Status: PASS");
}
```

## Version

Version 1.0 - Complete ESP32 hardware diagnostic suite