# 🛡️ Secure Cold Chain Vaccine Monitor (SCCVM-v1)

![Status](https://img.shields.io/badge/Status-Prototype-green)
![Framework](https://img.shields.io/badge/Framework-ESP--IDF-red)
![RTOS](https://img.shields.io/badge/OS-FreeRTOS-blue)
![Language](https://img.shields.io/badge/Language-C-00599C)

A distinctly engineered **IoT firmware** designed to monitor and protect vaccine cold-chain integrity.  
This project moves beyond *Arduino-style* loops by leveraging **FreeRTOS multitasking**, **SPIFFS-based crash-resistant storage**, and **secure MQTTS (TLS 1.2)** cloud telemetry.

This firmware is designed with **data integrity as the highest priority**, treating the device as a *black box recorder* for mission-critical medical logistics.



## 📖 Table of Contents
- [Project Overview](#-project-overview)
- [System Architecture](#-system-architecture)
- [Key Features](#-key-features)
- [Hardware Required](#-hardware-required)
- [Getting Started](#-getting-started)
- [Project Structure](#-project-structure)
- [The Dashboard](#-the-dashboard)
- [Future Roadmap](#-future-roadmap)
- [Contributing](#-contributing)
- [License](#-license)


## 🎯 Project Overview

In cold-chain logistics, vaccines must be stored within a strict temperature range (**2°C – 8°C**).  
Any data loss caused by power failure or network outages can result in **spoiled vaccines and financial loss**.

**SCCVM-v1** acts as a **black box temperature recorder**:

- If Wi-Fi is available → data is transmitted securely to the cloud  
- If Wi-Fi fails → data is **persisted locally in flash memory**  
- Once connectivity returns → real-time telemetry resumes automatically  

> **Connectivity is optional. Data integrity is mandatory.**


## 🏗 System Architecture

The firmware follows a **Producer–Consumer architecture** using FreeRTOS primitives.

```mermaid
graph TD
    A[Sensor Task] -->|Acquire Data|
    B[Vaccine Data Packet] -->|xQueueSend|
    C[FreeRTOS Queue] -->|xQueueReceive|
    D[Storage Task] -->|Binary Append|
    E[SPIFFS Flash] D -->|Publish|
    F[MQTT Task] -->|TLS 1.2|
    G[Cloud Broker] -->|WebSockets|
     H[Web Dashboard]

````

### Data Structure (Packed Binary)

To minimize flash usage and CPU overhead, data is stored as a **packed binary struct** instead of CSV or JSON.

```c
typedef struct __attribute__((packed)) {
    uint32_t timestamp;    // Unix Epoch
    float temperature;     // Critical Payload
    float humidity;        // Environmental Context
    uint8_t batteryLevel;  // Power Management
    uint8_t status;        // Error Flags
} VaccineData_t;
```


## ✨ Key Features

### 1. 🧠 Multi-Threaded Real-Time OS

* **Sensor Task** – High-priority data acquisition
* **Storage Task** – Event-driven consumer (sleeps when idle)
* **MQTT Task** – Secure network publishing
* Thread-safe communication via **FreeRTOS Queues**



### 2. 💾 Crash-Proof Persistence (Black Box Design)

* Custom **partition table** reserving flash for storage
* **SPIFFS** file system for resilience
* Continuous binary logging to `log.bin`
* **Self-healing boot process**:

  * File system integrity check
  * Historical data validation after reboot



### 3. 🔒 Military-Grade Security

* ❌ No plaintext MQTT (1883 disabled)
* ✅ Secure **MQTTS (8883)** using TLS 1.2
* ESP-IDF **CRT Bundle** for Root CA validation
* Encrypted communication end-to-end



### 4. 🌐 Real-Time Telemetry

* JSON telemetry published to MQTT broker
* **HTML5 Web Dashboard**
* WebSocket-based real-time updates



## 🛠 Hardware Required

* **MCU:** ESP32 Development Board (ESP-WROOM-32)
* **Cable:** Micro-USB data cable
* **Sensor:**

  * Simulated in v1
  * Compatible with DHT11 / DHT22 / DS18B20



## 🚀 Getting Started

### Prerequisites

* VS Code + **Espressif IDF Extension**
* **ESP-IDF v5.3.1
* Git



### Installation

#### 1️⃣ Clone the Repository

```bash
git https://github.com/HAKIMTABAKH/Vaccine-Monitor-Firmware.git
cd Vaccine-Monitor-Firmware
```

#### 2️⃣ Configure Wi-Fi Credentials

Edit:

```
main/wifi_connect.c
```

(or create `wifi_credentials.h` if refactored)

```c
#define ESP_WIFI_SSID "YOUR_WIFI_SSID"
#define ESP_WIFI_PASS "YOUR_WIFI_PASS"
```



#### 3️⃣ Build the Project

```bash
idf.py build
```

--

#### 4️⃣ Flash the Firmware

⚠️ **First flash requires a full erase** to apply the custom partition table.

```bash
idf.py -p COMx erase-flash
idf.py -p COMx flash monitor
```



## 📂 Project Structure

```text
sccvm-firmware/
├── partitions.csv          # Custom flash memory layout
├── CMakeLists.txt          # Root build configuration
├── main/
│   ├── include/
│   │   ├── types.h         # Data structures
│   │   ├── mqtt_app.h      # MQTT interface
│   │   └── wifi_connect.h  # Wi-Fi interface
│   ├── main.c              # Entry point & task scheduler
│   ├── wifi_connect.c      # Event-driven Wi-Fi state machine
│   ├── mqtt_app.c          # Secure MQTT client
│   └── CMakeLists.txt
└── web/
    └── dashboard.html      # Real-time monitoring dashboard
```



## 📊 The Dashboard

A **standalone HTML5 dashboard** is included in the `web/` directory.

### How to Use

* Open `dashboard.html` in any modern browser
* Connects via **Secure WebSockets (WSS)** to the MQTT broker

### Features

* Live temperature gauge
* Last synchronization timestamp
* 🔴 **RED ALERT overlay** if temperature exceeds 8°C
* Scrolling raw log terminal



## 🛣 Future Roadmap

* 🔋 **Deep Sleep Mode** – Reduce power consumption
* 🔄 **Offline Data Backfill** – Upload stored data after reconnection
* 🔁 **OTA Updates** – Secure firmware updates over Wi-Fi
* 📲 **Wi-Fi Provisioning** – Remove hardcoded credentials



## 🤝 Contributing

This project is an **architectural prototype** built for learning, demonstration, and portfolio purposes.
Contributions related to optimization, security, or new features are welcome.




