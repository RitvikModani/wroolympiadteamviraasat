

## 📌 About the Project

**Heritage Guardian** is an AI-Powered Robotic Cultural Experience & Protection System built for **WRO 2026: Robots Meet Culture**.

It addresses the growing problem of vandalism and visitor disengagement at cultural heritage sites by combining:

- 🤖 A line-following robot that guides and monitors visitors
- 📷 AI-based vandalism detection using an ESP32 camera
- 🔊 Virtual ultrasonic security fencing around sensitive structures
- 🅿️ Smart parking management with real-time LCD display
- 🌐 An AI web experience that immerses visitors in the monument's history


## 👥 Team — Viraasat-Bot

| Name | Role |
|---|---|
| Kavish Agarwal | Core idea development & system design |
| Raeyansh Agarwal | Research, concept structuring & communication |
| Ritvik Santosh Modani | Technical implementation & system integration |

**Age Group:** Junior (11–15)
**Country:** India
**Competition:** WRO 2026 — Future Innovators


## 🌐 Live Website

> 🔗 **https://web-weaver--kavrobot.replit.app/**

The website hosts the AI-powered cultural experience platform where visitors can upload a photo and receive a personalised historical story set in the era of the monument they are visiting.

---

## 🛠️ Hardware Components

| Component | Purpose |
|---|---|
| Arduino Uno | Main microcontroller — navigation & decision logic |
| ESP32-CAM | Real-time visual monitoring & vandalism detection |
| IR Sensors (x2) | Line following / path detection |
| Ultrasonic Sensor (HC-SR04) | Obstacle detection + virtual boundary fencing |
| BO Motors (x2) | Differential drive navigation |
| Servo Motor | Gate control / restriction mechanism |
| Motor Driver (L298N) | Motor speed and direction control |
| LCD Display (16x2) | Real-time parking availability display |
| Buzzer | Intrusion alert for virtual fencing |
| IR/Ultrasonic Parking Sensors | Per-bay vehicle occupancy detection |
| Jumper Wires & Chassis | Structural assembly and wiring |

---

## 💻 Software & Technology

| Tool / Platform | Purpose |
|---|---|
| Arduino IDE (C/C++) | Embedded robot control logic |
| ESP32 Arduino Library | Camera capture and WiFi communication |
| AI Image & Text API | Personalised historical experience generation |
| Replit | Web platform hosting for visitor AI experience |
| Cloud Image Comparison | Vandalism detection (reference vs. live image) |

---

## ⚙️ System Architecture

```
INPUT LAYER
├── IR Sensors         → Path detection
├── Ultrasonic Sensors → Obstacle avoidance + virtual fencing
├── ESP32 Camera       → Visual monitoring
└── Parking Sensors    → Bay occupancy detection

PROCESSING LAYER
├── Arduino Uno        → Real-time control & decisions
└── Cloud / AI Layer   → Image analysis + experience generation

OUTPUT LAYER
├── BO Motors          → Navigation
├── Servo Motor        → Gate restriction
├── LCD Display        → Parking availability
├── Buzzer             → Intrusion alert
└── Alerts             → Security notifications
```

---

## 🔁 Decision Logic

```
IF obstacle detected     → STOP
ELSE IF off path         → CORRECT
ELSE                     → MOVE FORWARD

IF perimeter breach      → TRIGGER ALARM + ALERT SECURITY
IF suspicious activity   → CAPTURE IMAGE + FLAG INCIDENT
IF parking sensor update → REFRESH LCD DISPLAY
```

---

## 🌍 Social Impact

Heritage Guardian creates impact across five dimensions:

- **Cultural Preservation** — reduces vandalism, protects monuments for future generations
- **Education & Awareness** — immersive AI experiences build emotional connection to heritage
- **Inclusivity** — priority parking for elderly and disabled visitors, offline fallback guidance
- **Community** — strengthens civic sense and collective responsibility for public spaces
- **Environment** — reduced traffic idling lowers pollution around heritage sites

**Concrete deployment:** Warangal Fort, Telangana — 13th-century Kakatiya dynasty monument facing ongoing vandalism and parking congestion.

---

## 📄 Project Report

The full WRO project report is included in this repository:
> 📎 `Heritage_Guardian_Final.docx`


## 📜 License

This project was created for educational purposes as part of WRO 2026 Future Innovators. All content is original work by Team Viraasat-Bot.

---

## 🙏 Acknowledgements

- World Robot Olympiad (WRO) for the inspiring theme *"Robots Meet Culture"*
- Archaeological Survey of India for publicly available heritage site information
- UNESCO World Heritage Centre for research resources

---

*© 2026 Team Viraasat-Bot — India*
