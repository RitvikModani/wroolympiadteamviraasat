/*
  ═══════════════════════════════════════════════════════════════
                    SMART PARKING SYSTEM v5.0
  ═══════════════════════════════════════════════════════════════
  
  Complete parking management system with:
  ✓ 4 parking slots with IR sensors
  ✓ LCD display with real-time status
  ✓ Entry gate that BLOCKS when full
  ✓ Exit gate that always works
  ✓ Ultrasonic car detection
  ✓ Automatic gate control
  
  ═══════════════════════════════════════════════════════════════
                        PIN CONFIGURATION
  ═══════════════════════════════════════════════════════════════
  
  IR SENSORS (Digital):
    Slot 1 → D6
    Slot 2 → D7
    Slot 3 → D4
    Slot 4 → D5
  
  ULTRASONIC SENSORS:
    Entry  → TRIG: D2, ECHO: D3
    Exit   → TRIG: D10, ECHO: D11
  
  SERVOS:
    Entry Gate → D8
    Exit Gate  → D9
  
  LCD (I2C):
    SDA → A4
    SCL → A5
  
  ═══════════════════════════════════════════════════════════════
*/

#include <Wire.h>
#include <LiquidCrystalI2C.h>
#include <Servo.h>

// ═══════════════════════════════════════════════════════════════
//                    HARDWARE CONFIGURATION
// ═══════════════════════════════════════════════════════════════

// LCD Setup (try 0x3F if 0x27 doesn't work)
LiquidCrystalI2C lcd(0x27, 16, 2);

// IR Sensor Pins
const byte IR_SLOT_1 = 6;
const byte IR_SLOT_2 = 7;
const byte IR_SLOT_3 = 4;
const byte IR_SLOT_4 = 5;

// Ultrasonic Sensor Pins
const byte ENTRY_TRIG = 10;
const byte ENTRY_ECHO = 11;
const byte EXIT_TRIG = 12;
const byte EXIT_ECHO = 13;

// Servo Pins
const byte ENTRY_SERVO_PIN = 3;
const byte EXIT_SERVO_PIN = 9;

// ═══════════════════════════════════════════════════════════════
//                    SYSTEM PARAMETERS
// ═══════════════════════════════════════════════════════════════

// IR Sensor Settings
const bool USE_PULLUP = true;
const byte IR_LOGIC_EMPTY = HIGH;  // Change to LOW if sensors work opposite
const byte SENSOR_SAMPLES = 5;     // Number of readings to average
const byte SAMPLE_DELAY_MS = 3;    // Delay between readings

// Ultrasonic Settings
const byte DETECTION_DISTANCE = 15;  // cm - detect cars within this range
const byte US_SAMPLES = 3;           // Readings to average
const byte CONFIRMATION_COUNT = 3;   // Confirmations needed before action

// Servo Settings
const byte GATE_CLOSED = 0;          // Closed angle
const byte GATE_OPEN = 90;           // Open angle
const unsigned int GATE_OPEN_DURATION = 3000;   // ms - how long gate stays open
const unsigned int MIN_GATE_INTERVAL = 1500;    // ms - min time between operations

// Display Settings
const unsigned int DISPLAY_REFRESH = 400;      // ms - LCD update rate
const unsigned int FULL_WARNING_TIME = 2000;   // ms - show FULL message duration

// ═══════════════════════════════════════════════════════════════
//                    GLOBAL VARIABLES
// ═══════════════════════════════════════════════════════════════

// Parking slot status (1 = empty, 0 = occupied)
struct ParkingSlot {
  byte pin;
  bool isEmpty;
};

ParkingSlot slots[4] = {
  {IR_SLOT_1, true},
  {IR_SLOT_2, true},
  {IR_SLOT_3, true},
  {IR_SLOT_4, true}
};

// Gate management
struct Gate {
  Servo motor;
  bool isOpen;
  unsigned long closeTime;
  unsigned long lastOperation;
  byte servoPin;
  const char* name;
};

Gate entryGate = {Servo(), false, 0, 0, ENTRY_SERVO_PIN, "ENTRY"};
Gate exitGate = {Servo(), false, 0, 0, EXIT_SERVO_PIN, "EXIT"};

// Detection counters
byte entryDetectionCount = 0;
byte exitDetectionCount = 0;

// Display management
unsigned long lastDisplayUpdate = 0;
unsigned long fullMessageStartTime = 0;
bool showingFullMessage = false;

// ═══════════════════════════════════════════════════════════════
//                    SETUP FUNCTION
// ═══════════════════════════════════════════════════════════════

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println(F("\n\n"));
  Serial.println(F("═══════════════════════════════════════════════════"));
  Serial.println(F("        SMART PARKING SYSTEM - INITIALIZING"));
  Serial.println(F("═══════════════════════════════════════════════════"));
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  displayBootSequence();
  
  // Configure IR sensor pins
  pinMode(IR_SLOT_1, USE_PULLUP ? INPUT_PULLUP : INPUT);
  pinMode(IR_SLOT_2, USE_PULLUP ? INPUT_PULLUP : INPUT);
  pinMode(IR_SLOT_3, USE_PULLUP ? INPUT_PULLUP : INPUT);
  pinMode(IR_SLOT_4, USE_PULLUP ? INPUT_PULLUP : INPUT);
  Serial.println(F("✓ IR Sensors configured on D6, D7, D4, D5"));
  
  // Configure ultrasonic pins
  pinMode(ENTRY_TRIG, OUTPUT);
  pinMode(ENTRY_ECHO, INPUT);
  pinMode(EXIT_TRIG, OUTPUT);
  pinMode(EXIT_ECHO, INPUT);
  digitalWrite(ENTRY_TRIG, LOW);
  digitalWrite(EXIT_TRIG, LOW);
  Serial.println(F("✓ Ultrasonic sensors configured"));
  Serial.println(F("  Entry: TRIG=D2, ECHO=D3"));
  Serial.println(F("  Exit:  TRIG=D10, ECHO=D11"));
  
  // Attach and initialize servos
  entryGate.motor.attach(ENTRY_SERVO_PIN);
  exitGate.motor.attach(EXIT_SERVO_PIN);
  entryGate.motor.write(GATE_CLOSED);
  exitGate.motor.write(GATE_CLOSED);
  Serial.println(F("✓ Servos initialized on D8 (Entry) and D9 (Exit)"));
  
  // Display configuration summary
  Serial.println(F("\n--- SYSTEM CONFIGURATION ---"));
  Serial.print(F("IR Empty Logic: "));
  Serial.println(IR_LOGIC_EMPTY == HIGH ? F("HIGH") : F("LOW"));
  Serial.print(F("Detection Distance: "));
  Serial.print(DETECTION_DISTANCE);
  Serial.println(F(" cm"));
  Serial.print(F("Gate Open Time: "));
  Serial.print(GATE_OPEN_DURATION / 1000);
  Serial.println(F(" seconds"));
  
  Serial.println(F("\n═══════════════════════════════════════════════════"));
  Serial.println(F("        SYSTEM READY - MONITORING PARKING"));
  Serial.println(F("═══════════════════════════════════════════════════\n"));
  
  delay(500);
}

// ═══════════════════════════════════════════════════════════════
//                    MAIN LOOP
// ═══════════════════════════════════════════════════════════════

void loop() {
  unsigned long currentTime = millis();
  
  // Update all parking slot sensors
  updateAllSlots();
  
  // Calculate statistics
  byte vacantCount = countVacantSlots();
  byte recommendedSlot = findBestSlot();
  
  // Update display periodically
  if (currentTime - lastDisplayUpdate >= DISPLAY_REFRESH) {
    // Clear full message after timeout
    if (showingFullMessage && (currentTime - fullMessageStartTime >= FULL_WARNING_TIME)) {
      showingFullMessage = false;
    }
    
    // Update display
    if (!showingFullMessage) {
      updateLCDDisplay(vacantCount, recommendedSlot);
    }
    
    lastDisplayUpdate = currentTime;
  }
  
  // ═════════ ENTRY GATE LOGIC ═════════
  int entryDistance = measureDistance(ENTRY_TRIG, ENTRY_ECHO);
  
  // Count consecutive detections
  if (entryDistance > 0 && entryDistance <= DETECTION_DISTANCE) {
    if (entryDetectionCount < CONFIRMATION_COUNT) {
      entryDetectionCount++;
    }
  } else {
    entryDetectionCount = 0;
  }
  
  // Car confirmed at entry
  if (entryDetectionCount >= CONFIRMATION_COUNT) {
    if (vacantCount > 0) {
      // Space available - open gate
      openGate(entryGate, currentTime);
    } else {
      // PARKING FULL - DENY ENTRY
      denyEntry();
    }
    entryDetectionCount = 0;  // Reset
  }
  
  // Auto-close entry gate
  autoCloseGate(entryGate, currentTime);
  
  // ═════════ EXIT GATE LOGIC ═════════
  int exitDistance = measureDistance(EXIT_TRIG, EXIT_ECHO);
  
  // Count consecutive detections
  if (exitDistance > 0 && exitDistance <= DETECTION_DISTANCE) {
    if (exitDetectionCount < CONFIRMATION_COUNT) {
      exitDetectionCount++;
    }
  } else {
    exitDetectionCount = 0;
  }
  
  // Car confirmed at exit - ALWAYS ALLOW
  if (exitDetectionCount >= CONFIRMATION_COUNT) {
    openGate(exitGate, currentTime);
    exitDetectionCount = 0;
  }
  
  // Auto-close exit gate
  autoCloseGate(exitGate, currentTime);
  
  // Small delay for system stability
  delay(10);
}

// ═══════════════════════════════════════════════════════════════
//                    SENSOR FUNCTIONS
// ═══════════════════════════════════════════════════════════════

// Read and update all parking slot statuses
void updateAllSlots() {
  for (byte i = 0; i < 4; i++) {
    slots[i].isEmpty = readIRSensor(slots[i].pin);
  }
}

// Read IR sensor with debouncing (majority voting)
bool readIRSensor(byte pin) {
  byte emptyReadings = 0;
  
  for (byte i = 0; i < SENSOR_SAMPLES; i++) {
    if (digitalRead(pin) == IR_LOGIC_EMPTY) {
      emptyReadings++;
    }
    if (SAMPLE_DELAY_MS > 0) {
      delay(SAMPLE_DELAY_MS);
    }
  }
  
  // Return true if majority say empty
  return (emptyReadings > SENSOR_SAMPLES / 2);
}

// Count vacant parking slots
byte countVacantSlots() {
  byte count = 0;
  for (byte i = 0; i < 4; i++) {
    if (slots[i].isEmpty) count++;
  }
  return count;
}

// Find best available slot (priority: 2→3→1→4)
byte findBestSlot() {
  const byte priority[4] = {1, 2, 0, 3};  // Indices for slots 2,3,1,4
  
  for (byte i = 0; i < 4; i++) {
    byte slotIndex = priority[i];
    if (slots[slotIndex].isEmpty) {
      return slotIndex + 1;  // Return 1-based slot number
    }
  }
  return 0;  // No slots available
}

// ═══════════════════════════════════════════════════════════════
//                    ULTRASONIC FUNCTIONS
// ═══════════════════════════════════════════════════════════════

// Measure distance with averaging
int measureDistance(byte trigPin, byte echoPin) {
  long totalDistance = 0;
  byte validReadings = 0;
  
  for (byte i = 0; i < US_SAMPLES; i++) {
    long dist = singleUltrasonicPing(trigPin, echoPin);
    
    if (dist > 0 && dist < 400) {  // Valid range
      totalDistance += dist;
      validReadings++;
    }
    delay(10);
  }
  
  if (validReadings == 0) return -1;
  return totalDistance / validReadings;
}

// Single ultrasonic measurement
long singleUltrasonicPing(byte trigPin, byte echoPin) {
  // Send trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Measure echo time
  long duration = pulseIn(echoPin, HIGH, 30000);  // 30ms timeout
  
  if (duration == 0) return -1;
  
  // Calculate distance in cm
  long distance = (duration * 343) / 20000;  // Speed of sound: 343 m/s
  return distance;
}

// ═══════════════════════════════════════════════════════════════
//                    GATE CONTROL FUNCTIONS
// ═══════════════════════════════════════════════════════════════

// Open gate with safety checks
void openGate(Gate &gate, unsigned long currentTime) {
  // Safety checks
  if (gate.isOpen) return;  // Already open
  if (currentTime - gate.lastOperation < MIN_GATE_INTERVAL) return;  // Too soon
  
  // Open the gate
  gate.motor.write(GATE_OPEN);
  gate.isOpen = true;
  gate.closeTime = currentTime + GATE_OPEN_DURATION;
  gate.lastOperation = currentTime;
  
  // Log to serial
  Serial.print(F(">>> "));
  Serial.print(gate.name);
  Serial.println(F(" GATE OPENED"));
}

// Auto-close gate after timeout
void autoCloseGate(Gate &gate, unsigned long currentTime) {
  if (gate.isOpen && currentTime >= gate.closeTime) {
    gate.motor.write(GATE_CLOSED);
    gate.isOpen = false;
    
    Serial.print(F(">>> "));
    Serial.print(gate.name);
    Serial.println(F(" GATE CLOSED"));
  }
}

// Deny entry when parking is full
void denyEntry() {
  showFullWarning();
  Serial.println(F("\n╔════════════════════════════════════════╗"));
  Serial.println(F("║   ENTRY DENIED - PARKING LOT FULL!    ║"));
  Serial.println(F("╚════════════════════════════════════════╝\n"));
}

// ═══════════════════════════════════════════════════════════════
//                    DISPLAY FUNCTIONS
// ═══════════════════════════════════════════════════════════════

// Boot sequence animation
void displayBootSequence() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("  SMART PARKING"));
  lcd.setCursor(0, 1);
  lcd.print(F("   SYSTEM v5.0"));
  delay(1500);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Custom Pin Mode"));
  lcd.setCursor(0, 1);
  lcd.print(F("IR: 6,7,4,5"));
  delay(1200);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Initializing"));
  for (byte i = 0; i < 3; i++) {
    lcd.print(F("."));
    delay(400);
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("  System Ready!"));
  lcd.setCursor(0, 1);
  lcd.print(F(" 4 Slots Active"));
  delay(1500);
  lcd.clear();
}

// Update LCD with current status
void updateLCDDisplay(byte vacant, byte recommendedSlot) {
  // Line 1: Vacant count and status indicators
  lcd.setCursor(0, 0);
  lcd.print(F("Vacant: "));
  lcd.print(vacant);
  lcd.print(F("/4 ["));
  
  // Visual slot indicators: O=empty, X=occupied
  for (byte i = 0; i < 4; i++) {
    lcd.print(slots[i].isEmpty ? 'O' : 'X');
  }
  lcd.print(F("]"));
  
  // Line 2: Guidance or full warning
  lcd.setCursor(0, 1);
  if (vacant > 0 && recommendedSlot > 0) {
    lcd.print(F("Go to Slot "));
    lcd.print(recommendedSlot);
    lcd.print(F("    "));
  } else {
    lcd.print(F("** FULL **      "));
  }
  
  // Serial monitoring output
  printStatusToSerial(vacant, recommendedSlot);
}

// Display full parking warning
void showFullWarning() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("** PARKING **"));
  lcd.setCursor(0, 1);
  lcd.print(F("**  FULL!  **"));
  showingFullMessage = true;
  fullMessageStartTime = millis();
}

// Print detailed status to serial monitor
void printStatusToSerial(byte vacant, byte recommendedSlot) {
  Serial.print(F("Status: "));
  Serial.print(vacant);
  Serial.print(F("/4 vacant | Slots: "));
  
  for (byte i = 0; i < 4; i++) {
    Serial.print(i + 1);
    Serial.print(F(":"));
    Serial.print(slots[i].isEmpty ? 'E' : 'O');
    if (i < 3) Serial.print(F(" "));
  }
  
  if (recommendedSlot > 0) {
    Serial.print(F(" | Recommend: Slot "));
    Serial.print(recommendedSlot);
  }
  
  Serial.println();
}

// ═══════════════════════════════════════════════════════════════
//                    END OF CODE
// ═══════════════════════════════════════════════════════════════
