# USB Ware

<p align="center">
  <img src="img/USB_Ware.jpg" alt="USB Ware" width="200"/>
</p>

**ESP32-S2 HID Automation and Control Firmware**

---

## 1. Introduction

**USB Ware** is a modular firmware developed for the **ESP32-S2** microcontroller, designed to transform the device into a **USB Human Interface Device (HID)** capable of executing programmable keyboard and mouse automation tasks.

The firmware integrates an **embedded web server** allowing users to select and trigger multiple automation routines via a browser-based control panel.

---

## 2. Web Control Interface

Upon power-up, the ESP32-S2 initializes a Wi-Fi access point (AP) under default credentials:

* **SSID:** `USB Ware`
* **Password:** `HID1000#`
* **Hidden:** `false`
* **Local IP Address:** `10.0.0.1`

Users may connect to this network and access the control dashboard through:

```
http://10.0.0.1
```

The web interface provides a structured selection form, allowing execution of predefined operational modes. A dynamic status display provides feedback regarding current activity or response.

---

## 3. Requirements

<p align="center">
  <img src="img/ESP32-S2-Mini-V1.0.0-LOLIN-WIFI-IOT-Board.png" alt="ESP32-S2 Mini" width="200"/>
</p>


| Component                   | Specification                                                                        |
| --------------------------- | ------------------------------------------------------------------------------------ |
| **Microcontroller**         | ESP32-S2 (native USB support required)                                               |
| **Development Environment** | Arduino IDE 1.8.19 or later                                                          |
| **Core Version**            | ESP32 Arduino Core ≥ 3.3.0                                                           |
| **Libraries**               | `WiFi.h`, `WebServer.h`, `DNSServer.h`, `USB.h`, `USBHIDKeyboard.h`, `USBHIDMouse.h` |

---

## 4. Future Enhancements

Potential extensions include:

* Local payload storage using **SPIFFS**.
* Over-The-Air (OTA) firmware updates.
* Enhanced telemetry and console output.
