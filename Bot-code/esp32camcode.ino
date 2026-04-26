#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Replace with your WiFi credentials
const char* ssid = "FTTH-53DB";
const char* password = "12345678";

// Replace with your Teachable Machine model endpoint
String serverName = "https://teachablemachine.withgoogle.com/models/2CBt7kl7_/";

HardwareSerial mySerial(1); // Use UART1 for communication with Arduino

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, 14, 15); // TX=14, RX=15 (adjust pins if needed)

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Camera configuration for AI Thinker ESP32-CAM
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Initialize camera
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    return;
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "image/jpeg");

    int httpResponseCode = http.POST(fb->buf, fb->len);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);

      if (response.indexOf("vandalism") >= 25) {
        mySerial.println("ALERT");
        Serial.println("ALERT sent to Arduino");
      } else {
        mySerial.println("clear");
        Serial.println("clear sent to Arduino");
      }
    } else {
      Serial.printf("Error code: %d\n", httpResponseCode);
    }

    http.end();
    esp_camera_fb_return(fb);
  }

  delay(2000); // Wait before next capture
}
