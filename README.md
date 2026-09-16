# 🔒 Smart E-Paper Access Control System
> An interactive Arduino-based IoT smart lock system featuring multi-bus hardware integration, real-time ultrasonic proximity detection, and dynamic E-Paper emotional feedback.

---

## 📌 Project Overview
This project presents an integrated smart access control system built on the Arduino platform. Beyond traditional authentication, it focuses on **Context-Aware Interaction** and **Emotional UI Design** by using an E-Paper display to show real-time animated feedback (Minion-themed expressions) based on authentication states.

### 🌟 Key Features
* **Proximity Detection:** Ultrasonic sensor automatically detects approaching users and switches system state from Standby to Ready.
* **Dual Display Architecture:**
  * **OLED (I2C):** Displays real-time high-frequency system telemetry and operational instructions.
  * **E-Paper (SPI):** Low-power display presenting dynamic facial expressions for user emotional feedback.
* **Secure Authentication:** RFID (RC522) UID identification driving an active relay module to trigger physical electronic lock hardware.
* **Multi-Bus Hardware Optimization:** Efficiently manages shared SPI bus communication (RFID & E-Paper) without pin contention or data corruption.

---

## 🛠️ System Architecture & Pin Layout

### Hardware Components
* **MCU:** Arduino Mega 2560 (compatible with Arduino Micro)
* **Authentication:** MFRC522 RFID Module
* **Displays:** 0.96" I2C OLED (SSD1306) & 1.54" SPI E-Paper (GxEPD2 supported)
* **Sensors & Actuators:** HC-SR04 Ultrasonic Sensor, Relay Module, Common-Cathode RGB LED

### Pin Mapping

| Peripheral | Component Pin | Arduino Pin | Communication / Note |
| :--- | :--- | :--- | :--- |
| **RFID (RC522)** | SDA (CS) | **Pin 53** | Hardware SPI (Dedicated CS) |
| | MOSI | **Pin 51** | Shared SPI Bus |
| | SCK | **Pin 52** | Shared SPI Bus |
| **E-Paper 1.54"**| CS | **Pin 4** | Hardware SPI (Dedicated CS) |
| | DIN (MOSI) | **Pin 51** | Shared SPI Bus |
| | CLK (SCK) | **Pin 52** | Shared SPI Bus |
| | DC / RST / BUSY | **Pin 6 / 12 / 13** | Digital I/O Control |
| **OLED 0.96"** | SDA / SCL | **SDA / SCL** | I2C Protocol (0x3C) |
| **Ultrasonic** | Trig / Echo | **Pin 2 / Pin 3** | Digital I/O |
| **Feedback** | Relay / RGB LED | **Pin 7 / Pin 9, 10, 11** | Actuator & Status Indicator |

---

## 🔄 System Finite State Machine (FSM)
[ Standby State ] ──( Distance < 15cm )──> [ Person Detected ]
▲                                          │
│                                    ( Scan RFID )
│                                          │
├───────( Access Granted )───────────[ Verify UID ]
│  • RGB: Green                            │
│  • E-Paper: Happy Expression             ├───────( Access Denied )
│  • Relay: Unlocked (3s)                  │  • RGB: Flash Red 3x
│                                          │  • E-Paper: Angry Expression
└──────────────────────────────────────────┴──• Relay: Locked



