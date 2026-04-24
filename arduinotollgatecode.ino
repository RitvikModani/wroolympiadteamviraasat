#include <Servo.h>

// === Pin Connections ===
// Servo motor signal wire → Arduino Uno pin 11
// Servo motor power → 5V
// Servo motor ground → GND
//
// Buzzer positive → Arduino Uno pin 8
// Buzzer negative → GND
//
// ESP32-CAM TX → Arduino Uno RX (pin 0)
// ESP32-CAM RX → Arduino Uno TX (pin 1)
// Common GND between ESP32-CAM and Arduino Uno

#define SERVO_PIN 11
#define BUZZER_PIN 8

Servo gateServo;
String inputString = "";

void setup() {
  Serial.begin(9600);          // Serial communication with ESP32-CAM
  gateServo.attach(SERVO_PIN); // Attach servo to pin 11
  gateServo.write(90);         // Default gate position = open (90°)
  
  pinMode(BUZZER_PIN, OUTPUT); // Buzzer setup
  digitalWrite(BUZZER_PIN, LOW); // Buzzer OFF initially
}

void loop() {
  if (Serial.available()) {
    inputString = Serial.readStringUntil('\n'); // Read message from ESP32-CAM

    if (inputString == "ALERT") {
      gateServo.write(0);              // Close gate (servo at 0°)
      digitalWrite(BUZZER_PIN, HIGH);  // Turn buzzer ON
      Serial.println("Security Alert: Gate locked, buzzer ON!");
    } 
    else if (inputString == "clear") {
      gateServo.write(90);             // Open gate (servo at 90°)
      digitalWrite(BUZZER_PIN, LOW);   // Turn buzzer OFF
      Serial.println("Visitor clear: Gate open, buzzer OFF.");
    }
  }
}
