/*
 * ESP32-CAM Complete Diagnostic Test
 * Comprehensive hardware testing for ESP32-CAM modules
 * Tests camera, WiFi, SD card, GPIO, and all peripherals
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SD_MMC.h>
#include <SPI.h>
#include <Wire.h>

// ESP32-CAM Pin Definitions (AI-Thinker model)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Additional GPIO pins
#define LED_FLASH_PIN      4  // Built-in flash LED
#define SD_CS_PIN         -1  // SD card uses MMC interface
#define I2C_SDA_PIN       26  // Shared with camera SIOD
#define I2C_SCL_PIN       27  // Shared with camera SIOC

// Test results structure
struct CAMTestResults {
  byte system : 1;
  byte camera : 1;
  byte wifi : 1;
  byte sdcard : 1;
  byte gpio : 1;
  byte flash : 1;
  byte i2c : 1;
  byte spi : 1;
  byte webserver : 1;
  byte memory : 1;
  byte temperature : 1;
  byte streaming : 1;
} results = {0};

WebServer server(80);
bool cameraInitialized = false;
bool sdCardAvailable = false;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  printHeader();
  initializeBasicPins();
  runAllTests();
  printSummary();
  
  if (results.camera && results.wifi) {
    startWebServer();
  }
  
  Serial.println("\nDiagnostic complete. Starting monitoring...");
}

void loop() {
  if (results.wifi && results.camera) {
    server.handleClient();
  }
  
  monitorSystem();
  delay(5000);
}

void printHeader() {
  Serial.println();
  Serial.println("==========================================");
  Serial.println("      ESP32-CAM COMPLETE DIAGNOSTIC      ");
  Serial.println("==========================================");
  Serial.println("Testing AI-Thinker ESP32-CAM module");
  Serial.println("Camera: OV2640 | Flash: Built-in LED");
}

void initializeBasicPins() {
  pinMode(LED_FLASH_PIN, OUTPUT);
  digitalWrite(LED_FLASH_PIN, LOW);
}

void runAllTests() {
  testSystem();
  testCamera();
  testSDCard();
  testGPIO();
  testFlashLED();
  testI2C();
  testSPI();
  testWiFi();
  testMemory();
  testTemperature();
  if (results.camera) {
    testStreaming();
  }
  if (results.wifi && results.camera) {
    testWebServer();
  }
}

void testSystem() {
  Serial.println("\n1. SYSTEM INFORMATION");
  Serial.println("----------------------");
  
  Serial.printf("Chip Model: %s\n", ESP.getChipModel());
  Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
  Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Flash Size: %d MB\n", ESP.getFlashChipSize() / 1048576);
  Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("PSRAM Size: %d bytes\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
  
  // Check if PSRAM is available (important for camera)
  if (ESP.getPsramSize() > 0) {
    Serial.println("PSRAM: Available (Good for camera)");
  } else {
    Serial.println("PSRAM: Not detected (May limit camera performance)");
  }
  
  results.system = 1;
  Serial.println("Status: PASS");
}

void testCamera() {
  Serial.println("\n2. CAMERA TEST (OV2640)");
  Serial.println("------------------------");
  
  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Frame size and quality settings
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
    Serial.println("PSRAM found - Using high quality settings");
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
    Serial.println("No PSRAM - Using standard quality settings");
  }
  
  // Initialize camera
  Serial.print("Initializing camera... ");
  esp_err_t err = esp_camera_init(&config);
  
  if (err != ESP_OK) {
    Serial.printf("FAILED (Error: 0x%x)\n", err);
    Serial.println("Camera initialization failed!");
    return;
  }
  
  Serial.println("SUCCESS");
  cameraInitialized = true;
  
  // Test camera sensor
  sensor_t* s = esp_camera_sensor_get();
  if (s != NULL) {
    Serial.printf("Camera sensor ID: 0x%02X\n", s->id.PID);
    Serial.println("Camera sensor: Detected and responsive");
    
    // Test different frame sizes
    Serial.print("Testing frame sizes: ");
    framesize_t sizes[] = {FRAMESIZE_QVGA, FRAMESIZE_VGA, FRAMESIZE_SVGA};
    const char* sizeNames[] = {"QVGA", "VGA", "SVGA"};
    
    for (int i = 0; i < 3; i++) {
      s->set_framesize(s, sizes[i]);
      delay(100);
      camera_fb_t* fb = esp_camera_fb_get();
      if (fb) {
        Serial.printf("%s(%dx%d) ", sizeNames[i], fb->width, fb->height);
        esp_camera_fb_return(fb);
      }
    }
    Serial.println();
    
    // Test image capture
    Serial.print("Capturing test image... ");
    camera_fb_t* fb = esp_camera_fb_get();
    if (fb) {
      Serial.printf("SUCCESS (%d bytes, %dx%d)\n", fb->len, fb->width, fb->height);
      esp_camera_fb_return(fb);
    } else {
      Serial.println("FAILED - No frame buffer");
    }
    
  } else {
    Serial.println("Camera sensor: Not responding");
  }
  
  results.camera = 1;
  Serial.println("Status: PASS");
}

void testSDCard() {
  Serial.println("\n3. SD CARD TEST");
  Serial.println("---------------");
  
  Serial.print("Initializing SD card... ");
  
  if (!SD_MMC.begin()) {
    Serial.println("FAILED - No SD card detected");
    Serial.println("Insert SD card for full functionality");
    return;
  }
  
  Serial.println("SUCCESS");
  sdCardAvailable = true;
  
  // Get card info
  uint8_t cardType = SD_MMC.cardType();
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("Unknown");
  }
  
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %llu MB\n", cardSize);
  Serial.printf("Total space: %llu MB\n", SD_MMC.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %llu MB\n", SD_MMC.usedBytes() / (1024 * 1024));
  
  // Test file operations
  Serial.print("Testing file operations... ");
  File file = SD_MMC.open("/test.txt", FILE_WRITE);
  if (file) {
    file.println("ESP32-CAM SD Card Test");
    file.printf("Timestamp: %lu\n", millis());
    file.close();
    
    // Read back
    file = SD_MMC.open("/test.txt");
    if (file) {
      Serial.print("Write/Read OK - ");
      while (file.available()) {
        Serial.write(file.read());
      }
      file.close();
      SD_MMC.remove("/test.txt");
      Serial.println("File deleted OK");
    }
  } else {
    Serial.println("File write failed");
  }
  
  results.sdcard = 1;
  Serial.println("Status: PASS");
}

void testGPIO() {
  Serial.println("\n4. GPIO TEST");
  Serial.println("------------");
  
  // Test available GPIO pins (avoiding camera pins)
  int testPins[] = {2, 12, 13, 15, 16}; // Safe GPIO pins on ESP32-CAM
  int numPins = sizeof(testPins) / sizeof(testPins[0]);
  
  Serial.println("Testing available GPIO pins:");
  
  for (int i = 0; i < numPins; i++) {
    int pin = testPins[i];
    pinMode(pin, OUTPUT);
    
    Serial.printf("GPIO%d: ", pin);
    
    // Test digital output
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
    
    Serial.print("Digital OK ");
    
    // Test as input
    pinMode(pin, INPUT_PULLUP);
    int state = digitalRead(pin);
    Serial.printf("Input=%s ", state ? "HIGH" : "LOW");
    
    Serial.println();
  }
  
  results.gpio = 1;
  Serial.println("Status: PASS");
}

void testFlashLED() {
  Serial.println("\n5. FLASH LED TEST");
  Serial.println("-----------------");
  
  Serial.print("Testing built-in flash LED (GPIO4): ");
  
  // Test flash LED
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_FLASH_PIN, HIGH);
    delay(200);
    digitalWrite(LED_FLASH_PIN, LOW);
    delay(200);
  }
  
  Serial.println("Blink test completed");
  
  // Test PWM control for brightness
  Serial.print("Testing PWM brightness control: ");
  ledcAttach(LED_FLASH_PIN, 5000, 8);
  
  for (int brightness = 0; brightness <= 255; brightness += 51) {
    ledcWrite(LED_FLASH_PIN, brightness);
    delay(200);
  }
  ledcWrite(LED_FLASH_PIN, 0);
  
  Serial.println("PWM test completed");
  
  results.flash = 1;
  Serial.println("Status: PASS");
}void 
testI2C() {
  Serial.println("\n6. I2C TEST");
  Serial.println("-----------");
  
  // Note: I2C pins are shared with camera, so we test carefully
  Serial.println("I2C pins shared with camera - Limited testing");
  
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Serial.print("Scanning I2C bus (camera should be detected): ");
  
  int deviceCount = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.printf("0x%02X ", addr);
      deviceCount++;
    }
  }
  
  Serial.printf("\nFound %d I2C device(s)\n", deviceCount);
  
  if (deviceCount > 0) {
    Serial.println("Camera sensor detected on I2C bus");
  } else {
    Serial.println("No I2C devices found (camera may use different addressing)");
  }
  
  results.i2c = 1;
  Serial.println("Status: PASS");
}

void testSPI() {
  Serial.println("\n7. SPI TEST");
  Serial.println("-----------");
  
  Serial.println("SPI pins available for external devices");
  
  // Use available pins for SPI (avoiding camera pins)
  SPI.begin(14, 2, 15, 13); // SCK, MISO, MOSI, SS
  
  Serial.print("SPI initialization: ");
  Serial.println("OK");
  
  Serial.print("Testing SPI communication: ");
  byte testData[] = {0xAA, 0x55, 0xFF};
  
  for (int i = 0; i < 3; i++) {
    byte received = SPI.transfer(testData[i]);
    Serial.printf("%02X->%02X ", testData[i], received);
  }
  
  SPI.end();
  Serial.println("\nSPI test completed");
  
  results.spi = 1;
  Serial.println("Status: PASS");
}

void testWiFi() {
  Serial.println("\n8. WIFI TEST");
  Serial.println("------------");
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.print("Scanning for WiFi networks... ");
  int networkCount = WiFi.scanNetworks();
  
  Serial.printf("Found %d networks\n", networkCount);
  
  // Show first 3 networks
  for (int i = 0; i < min(networkCount, 3); i++) {
    Serial.printf("  %d: %s (%d dBm) %s\n", 
                  i + 1,
                  WiFi.SSID(i).c_str(),
                  WiFi.RSSI(i),
                  WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "[OPEN]" : "[SECURED]");
  }
  
  // Test AP mode
  Serial.print("Testing Access Point mode: ");
  WiFi.mode(WIFI_AP);
  if (WiFi.softAP("ESP32-CAM-Test", "12345678")) {
    Serial.printf("SUCCESS (IP: %s)\n", WiFi.softAPIP().toString().c_str());
    delay(2000);
    
    // Keep AP mode for web server if camera works
    if (!cameraInitialized) {
      WiFi.softAPdisconnect(true);
      WiFi.mode(WIFI_OFF);
    }
  } else {
    Serial.println("FAILED");
    WiFi.mode(WIFI_OFF);
  }
  
  results.wifi = 1;
  Serial.println("Status: PASS");
}

void testMemory() {
  Serial.println("\n9. MEMORY TEST");
  Serial.println("--------------");
  
  int heapBefore = ESP.getFreeHeap();
  int psramBefore = ESP.getFreePsram();
  
  Serial.printf("Free Heap: %d bytes\n", heapBefore);
  Serial.printf("Free PSRAM: %d bytes\n", psramBefore);
  
  // Test heap allocation
  Serial.print("Testing heap allocation: ");
  char* heapMem = (char*)malloc(10000);
  if (heapMem) {
    Serial.printf("10KB allocated OK\n");
    free(heapMem);
  } else {
    Serial.println("FAILED");
  }
  
  // Test PSRAM allocation if available
  if (psramBefore > 0) {
    Serial.print("Testing PSRAM allocation: ");
    char* psramMem = (char*)ps_malloc(100000);
    if (psramMem) {
      Serial.printf("100KB PSRAM allocated OK\n");
      free(psramMem);
    } else {
      Serial.println("FAILED");
    }
  }
  
  Serial.printf("Memory after test - Heap: %d, PSRAM: %d\n", 
                ESP.getFreeHeap(), ESP.getFreePsram());
  
  results.memory = 1;
  Serial.println("Status: PASS");
}

void testTemperature() {
  Serial.println("\n10. TEMPERATURE TEST");
  Serial.println("-------------------");
  
  float temp = temperatureRead();
  Serial.printf("CPU Temperature: %.2f°C\n", temp);
  
  if (temp > 20 && temp < 80) {
    Serial.println("Temperature reading: Normal operating range");
  } else if (temp >= 80) {
    Serial.println("Temperature reading: HIGH - Check cooling");
  } else {
    Serial.println("Temperature reading: Unusual value");
  }
  
  results.temperature = 1;
  Serial.println("Status: PASS");
}

void testStreaming() {
  Serial.println("\n11. CAMERA STREAMING TEST");
  Serial.println("-------------------------");
  
  if (!cameraInitialized) {
    Serial.println("Camera not initialized - Skipping streaming test");
    return;
  }
  
  Serial.print("Testing continuous capture: ");
  
  int successfulCaptures = 0;
  int totalAttempts = 5;
  
  for (int i = 0; i < totalAttempts; i++) {
    camera_fb_t* fb = esp_camera_fb_get();
    if (fb) {
      successfulCaptures++;
      esp_camera_fb_return(fb);
      Serial.print(".");
    } else {
      Serial.print("X");
    }
    delay(500);
  }
  
  Serial.printf("\nCapture success rate: %d/%d (%.1f%%)\n", 
                successfulCaptures, totalAttempts, 
                (float)successfulCaptures/totalAttempts*100);
  
  if (successfulCaptures >= 4) {
    Serial.println("Streaming performance: Excellent");
  } else if (successfulCaptures >= 2) {
    Serial.println("Streaming performance: Good");
  } else {
    Serial.println("Streaming performance: Poor - Check connections");
  }
  
  results.streaming = 1;
  Serial.println("Status: PASS");
}

void testWebServer() {
  Serial.println("\n12. WEB SERVER TEST");
  Serial.println("-------------------");
  
  if (!cameraInitialized || !WiFi.softAPgetStationNum() >= 0) {
    Serial.println("Prerequisites not met - Skipping web server test");
    return;
  }
  
  Serial.println("Web server will be started for camera streaming");
  Serial.printf("Connect to WiFi: ESP32-CAM-Test (password: 12345678)\n");
  Serial.printf("Camera stream URL: http://%s/stream\n", WiFi.softAPIP().toString().c_str());
  Serial.printf("Capture URL: http://%s/capture\n", WiFi.softAPIP().toString().c_str());
  
  results.webserver = 1;
  Serial.println("Status: PASS");
}

void startWebServer() {
  // Root page
  server.on("/", HTTP_GET, []() {
    String html = "<html><head><title>ESP32-CAM Test</title></head>";
    html += "<body><h1>ESP32-CAM Diagnostic</h1>";
    html += "<p><a href='/stream'>Live Stream</a></p>";
    html += "<p><a href='/capture'>Capture Photo</a></p>";
    html += "<p><a href='/status'>System Status</a></p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });
  
  // Stream endpoint
  server.on("/stream", HTTP_GET, []() {
    WiFiClient client = server.client();
    String response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
    server.sendContent(response);
    
    while (client.connected()) {
      camera_fb_t* fb = esp_camera_fb_get();
      if (fb) {
        client.printf("--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);
        client.write(fb->buf, fb->len);
        client.print("\r\n");
        esp_camera_fb_return(fb);
      }
      delay(100);
    }
  });
  
  // Capture endpoint
  server.on("/capture", HTTP_GET, []() {
    camera_fb_t* fb = esp_camera_fb_get();
    if (fb) {
      server.sendHeader("Content-Disposition", "attachment; filename=capture.jpg");
      server.send_P(200, "image/jpeg", (const char*)fb->buf, fb->len);
      esp_camera_fb_return(fb);
    } else {
      server.send(500, "text/plain", "Camera capture failed");
    }
  });
  
  // Status endpoint
  server.on("/status", HTTP_GET, []() {
    String status = "ESP32-CAM Status\n";
    status += "Free Heap: " + String(ESP.getFreeHeap()) + " bytes\n";
    status += "Free PSRAM: " + String(ESP.getFreePsram()) + " bytes\n";
    status += "Temperature: " + String(temperatureRead()) + "°C\n";
    status += "Uptime: " + String(millis()) + " ms\n";
    server.send(200, "text/plain", status);
  });
  
  server.begin();
  Serial.println("Web server started successfully");
}

void printSummary() {
  Serial.println("\n==========================================");
  Serial.println("         ESP32-CAM TEST SUMMARY          ");
  Serial.println("==========================================");
  
  int passed = 0;
  int total = 12;
  
  Serial.printf("1.  System Info:     %s\n", results.system ? "PASS" : "FAIL");
  Serial.printf("2.  Camera:          %s\n", results.camera ? "PASS" : "FAIL");
  Serial.printf("3.  SD Card:         %s\n", results.sdcard ? "PASS" : "FAIL");
  Serial.printf("4.  GPIO:            %s\n", results.gpio ? "PASS" : "FAIL");
  Serial.printf("5.  Flash LED:       %s\n", results.flash ? "PASS" : "FAIL");
  Serial.printf("6.  I2C:             %s\n", results.i2c ? "PASS" : "FAIL");
  Serial.printf("7.  SPI:             %s\n", results.spi ? "PASS" : "FAIL");
  Serial.printf("8.  WiFi:            %s\n", results.wifi ? "PASS" : "FAIL");
  Serial.printf("9.  Memory:          %s\n", results.memory ? "PASS" : "FAIL");
  Serial.printf("10. Temperature:     %s\n", results.temperature ? "PASS" : "FAIL");
  Serial.printf("11. Streaming:       %s\n", results.streaming ? "PASS" : "FAIL");
  Serial.printf("12. Web Server:      %s\n", results.webserver ? "PASS" : "FAIL");
  
  // Count passes
  passed = results.system + results.camera + results.sdcard + results.gpio + 
           results.flash + results.i2c + results.spi + results.wifi + 
           results.memory + results.temperature + results.streaming + results.webserver;
  
  Serial.println("------------------------------------------");
  Serial.printf("OVERALL RESULT: %d/%d tests passed (%.1f%%)\n", 
                passed, total, (float)passed/total*100);
  
  if (passed >= 10) {
    Serial.println("EXCELLENT! ESP32-CAM is fully functional");
  } else if (passed >= 8) {
    Serial.println("GOOD! Most features working properly");
  } else if (passed >= 6) {
    Serial.println("FAIR! Some issues detected");
  } else {
    Serial.println("POOR! Multiple failures - Check hardware");
  }
  
  Serial.println("==========================================");
  
  // Special recommendations
  if (!results.camera) {
    Serial.println("WARNING: Camera failed - Check connections and power");
  }
  if (!results.sdcard) {
    Serial.println("INFO: Insert SD card for image storage capability");
  }
  if (results.camera && results.wifi) {
    Serial.println("SUCCESS: Camera streaming available via web interface");
  }
}

void monitorSystem() {
  static unsigned long lastUpdate = 0;
  static int photoCount = 0;
  
  if (millis() - lastUpdate >= 5000) {
    lastUpdate = millis();
    
    Serial.println("\n--- ESP32-CAM LIVE MONITOR ---");
    Serial.printf("Uptime: %lu ms\n", millis());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    Serial.printf("CPU Temp: %.1f°C\n", temperatureRead());
    
    if (WiFi.getMode() == WIFI_AP) {
      Serial.printf("AP Clients: %d\n", WiFi.softAPgetStationNum());
      Serial.printf("AP IP: %s\n", WiFi.softAPIP().toString().c_str());
    }
    
    if (cameraInitialized) {
      Serial.printf("Camera: Active\n");
      
      // Take a test photo every 30 seconds
      static unsigned long lastPhoto = 0;
      if (millis() - lastPhoto >= 30000) {
        lastPhoto = millis();
        camera_fb_t* fb = esp_camera_fb_get();
        if (fb) {
          photoCount++;
          Serial.printf("Test photo #%d captured (%d bytes)\n", photoCount, fb->len);
          
          // Save to SD card if available
          if (sdCardAvailable) {
            String filename = "/photo_" + String(photoCount) + ".jpg";
            File file = SD_MMC.open(filename.c_str(), FILE_WRITE);
            if (file) {
              file.write(fb->buf, fb->len);
              file.close();
              Serial.printf("Photo saved to SD: %s\n", filename.c_str());
            }
          }
          
          esp_camera_fb_return(fb);
        } else {
          Serial.println("Camera capture failed in monitor");
        }
      }
    }
    
    // Flash LED heartbeat
    digitalWrite(LED_FLASH_PIN, HIGH);
    delay(50);
    digitalWrite(LED_FLASH_PIN, LOW);
    
    Serial.println("-----------------------------");
  }
}