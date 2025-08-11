# USB_Ware

## Project Description

This project leverages the native USB capabilities of ESP32-S2/S3 boards to emulate a USB HID keyboard device. It automatically injects a Python script into a connected Windows host via a hidden PowerShell window. The injected script performs secure encryption or decryption of files located on the user's Desktop.

Key highlights include automatic installation of required Python libraries, secure key management, and stealth execution to minimize user disruption.

---

## Features

- **Native USB HID Keyboard Emulation:** Automates keystroke injection with high speed and reliability.
- **Stealth Mode Execution:** PowerShell is launched with the `-WindowStyle Hidden` flag to avoid visible windows.
- **Robust Python Script Injection:** Fully types out a Python script that handles file encryption/decryption.
- **Auto Library Installation:** Checks for the `cryptography` Python library and installs it if absent.
- **Dual Encryption/Decryption Modes:** Easily toggle between encrypting and decrypting files via a compile-time setting.
- **Secure Key Derivation & Storage:** Uses PBKDF2 with SHA-256 for key derivation and backs up keys securely.
- **Hidden Script File:** The injected Python file is marked as hidden to avoid accidental user deletion.
- **Configurable Injection Speed:** Adjustable typing delay ensures compatibility with various host machine speeds.

---

## Supported Hardware

- **ESP32-S2 and ESP32-S3 Development Boards:** Examples include:
  - ESP32-S2-Saola-1
  - ESP32-S3-DevKitC
- Boards with native USB HID capabilities are required for seamless injection.
- May be adapted for other native USB Arduino boards (e.g., Leonardo, Micro), but ESP32-S2/S3 offers superior performance.

---

## Requirements

- **Target Machine:**
  - Windows OS with PowerShell installed (default on Windows 10 and later)
  - Python 3.x installed and accessible via `python` command
  - Internet connection to install Python packages if missing

- **Host Development:**
  - Arduino IDE 1.8.19 or later
  - ESP32 Arduino core (latest recommended)

---

## Setup and Usage

1. **Configure Mode:**
   

   Open the Arduino sketch and set the mode:

   ```cpp
   const char* MODE = "e";  // "e" for encrypt, "d" for decrypt

---

**By:** dfyR433  
**Platform:** ESP32-S2 / ESP32-S3 (Native USB HID)  
**Languages:** Arduino (C++) and Python

---
