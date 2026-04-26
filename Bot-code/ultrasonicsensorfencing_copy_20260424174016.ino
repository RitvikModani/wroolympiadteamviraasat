// === Pin Connections ===
// Ultrasonic Sensor HC-SR04:
//   VCC → 5V
//   GND → GND
//   TRIG → Arduino Uno pin 9
//   ECHO → Arduino Uno pin 10
//
// Buzzer:
//   Positive → Arduino Uno pin 8
//   Negative → GND

#define TRIG_PIN 9
#define ECHO_PIN 10
#define BUZZER_PIN 8

long duration;
int distance;
int baselineDistance;   // Reference distance (fence line)

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);

  // Take initial baseline measurement
  baselineDistance = measureDistance();
  Serial.print("Baseline distance set to: ");
  Serial.println(baselineDistance);
}

void loop() {
  distance = measureDistance();
  Serial.print("Current distance: ");
  Serial.println(distance);

  // If distance changes significantly (object detected)
  if (abs(distance - baselineDistance) > 10) { // threshold = 10 cm
    digitalWrite(BUZZER_PIN, HIGH); // Buzzer ON
    Serial.println("Intrusion detected! Buzzer ON.");
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // Buzzer OFF
  }

  delay(500); // Half-second delay between checks
}

// Function to measure distance using ultrasonic sensor
int measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  int dist = duration * 0.034 / 2; // Convert to cm
  return dist;
}
//pls change the treshhold(baselineDistance >10 ) based on sensitivity of sensors
