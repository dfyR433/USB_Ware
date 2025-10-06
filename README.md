# One Click

<p align="center">
  <img src="logo.jpg" alt="USB Ware Logo" width="200"/>
</p>

## Overview

This project demonstrates how an ESP32 can:

* Host a Wi-Fi access point (AP) and act as a captive portal using DNS redirection.
* Serve simple HTML pages (demo login pages).
* Accept form submissions and log them to the serial console for demonstration only.
* Perform harmless HID actions (open Notepad and type a demo message), purely for demonstration.

> **Important:** The codebase has been sanitized for safety. Any original pages that mimicked real services or captured real credentials were replaced with clearly labeled demo pages and logging to Serial only.

---

## Features (Safe Variants)

Implemented safe/demo features:

* `captive_portal` — AP + DNS redirect to demo login page. **Demo only**; submitted values are printed to Serial (not saved or transmitted).
* `richroll` — Opens the configured URL on the host computer via HID (harmless link or demo site). Use with permission.
* `barrel_roll` — Opens a benign web query (fun demo).
* `note` — Opens Notepad and types a harmless message.
* `script` — Launches an embedded Python demo saved to the user profile and executes it; **the demo only manipulates test files** inside a controlled directory (no destructive behavior).
* `script_richroll`, `all-brute_force` — Combined safe demos. Note brute-force functionality is intentionally disabled; brute-force attack code is not present.

**Removed / Disabled:** Any code that mimics major services (Google, banking, etc.), code that exfiltrates credentials, or encryption/ransomware payloads. Brute-force and unauthorized scanning tools are not supported.

---

## Hardware & Software Requirements

* **Board:** ESP32 development board with USB HID capability or an ESP32 variant supported by your HID library. (Example: ESP32-S2/S3 if using HID libraries that support them).
* **USB HID library:** A compatible USB HID library for your board (the sample used `USB.h` and `USBHIDKeyboard.h` — pick the version compatible with your core and board).
* **Arduino IDE:**
* **Required libraries:**

  * `WebServer.h`
  * `DNSServer.h`
  * HID library compatible with your board
* **Host OS:** Windows is used in the demo for the HID example (opening Notepad / PowerShell). Adjust scripts for other OSes if needed.

---

## Quick Start

1. **Open the project** in Arduino IDE or PlatformIO.
2. **Select** the proper ESP32 board and COM port.
3. **Edit configuration** at the top of the sketch to choose a safe feature:

```cpp
// ================= User Settings =================

// Select feature:
// Options:
// "richroll"           -> Rickroll YouTube prank
// "captive_portal"     -> Launch captive portal with DNS redirection
// "script"             -> Execute embedded HID scripts
// "script_richroll"    -> Script + Rickroll
// "barrel_roll"        -> Barrel roll animation/demo
// "note"               -> Type notes automatically
// "brute_force"        -> Android PIN brute force demo
// "all-brute_force"    -> All except brute force
const char* feature = "richroll";

// Brute-force delay in milliseconds per attempt
int bruteDelay = 100;

// Captive portal Wi-Fi AP settings
String inputSSID = "Captive portal";  // SSID
String inputPASS = "12345678";       // Password (leave empty for open network)
String portalName = "super portal";  // Portal name (super portal, super google, super pass)

// Encryption/Decryption mode: "e" = encrypt, "d" = decrypt
const char* MODE = "e";

// Target folder inside user profile (Desktop, Documents, Downloads, etc.)
const char* TARGET_FOLDER_NAME = "Desktop";

// Typing delay multiplier (affects HID typing speed)
float Delay = 35.0;

// YouTube URL for Rickroll payload
const char* linkURL = "https://www.youtube.com/watch?v=dQw4w9WgXcQ";

//--------------------------------------------------
```

4. **Upload** the sketch to the ESP32.

---

## Configuration (Detailed)

Top-of-file variables you can safely edit:

* `feature` — Choose a demo feature. Example values:

  * `"captive_portal"` — demo captive portal.
  * `"richroll"` — open a demo URL via HID (use a harmless URL).
  * `"barrel_roll"`, `"note"`, `"script"`.
* `bruteDelay` — Only relevant if you enable a local demo loop. There is no brute-force enabled by default.
* `inputSSID` / `inputPASS` — SSID and password for the demo AP.
* `portalName` — Controls which demo HTML page is served (all pages are clearly labeled as demos).
* `MODE` — For the demo Python script, `"e"` or `"d"` (simulated; script will operate only on a test directory).
* `TARGET_FOLDER_NAME` — For the embedded script, use a safe test folder (e.g., `ESP32_Demo_Files`).
* `Delay` — Controls HID typing speed.

---

## Contributing

* Create issues for bug reports or feature requests.
* Pull requests must adhere to the project’s safety policy (no code for non-consensual credential capture, ransomware, destructive payloads, or unlawful activity).
* Provide test cases and documentation for any new demo features.

---

## License

This project is released under the **MIT License**. By using or contributing, you agree not to use the project for unlawful or unethical activities.
