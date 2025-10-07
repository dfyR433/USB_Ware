# USB Ware

<p align="center">
  <img src="logo.jpg" alt="USB Ware Logo" width="200"/>
</p>

**USB Ware** is an ESP-based firmware that exposes a small web UI to trigger HID actions (keyboard/mouse) and demo scripts on a connected Windows host. It was created as an educational tool and proof-of-concept to show how HID injection and scripted payloads can be triggered from a local Wi‑Fi interface.

> ⚠️ **Safety & Ethics**
>
> This project contains functionality that can be used to automate actions on a connected computer (opening apps, typing commands, writing files, running PowerShell/Python scripts, encrypting/decrypting files). Use **only** in controlled, isolated, and consented environments (your own test machine or lab). Do **not** deploy this on other machines or use it to access systems without explicit permission — doing so may be illegal.

---

## Contents

* `latest_release.ino` — main Arduino/ESP sketch (webserver + handlers).
* `Configs.h` — global configuration and embedded web UI HTML (`MainPage[]`).
* `USB_Tools.h` — HID helper functions and payload implementations (Python demo, Rickroll, barrel-roll, notepad, etc.).

---

## Features

* Wi‑Fi AP hosted configuration portal (defaults in `Configs.h`).
* Minimal web UI (neon terminal look) for triggering features:

  * **Ping Site** — opens a target URL (defaults to a YouTube Rickroll).
  * **Python Script** — types a multi-line PowerShell block which saves and runs a Python script (educational encrypt/decrypt demo).
  * **Script + Ping** — runs the Python script then opens the URL.
  * **Barrel Roll** — opens a Google query that triggers the barrel-roll animation.
  * **Notepad** — opens Notepad and types a short message.
  * **All** — run all enabled actions (sequence depends on the sketch).
* HID utilities implemented in `USB_Tools.h` including typed input, combos, opening Run dialog, switching windows, and more.

---

## Requirements

* Arduino IDE (1.8.x or later recommended) or PlatformIO.
* ESP32 development board with native USB/HID support (ESP32-S2 / ESP32-S3 recommended) if you want direct native USB HID functionality. Non-native-USB ESP32 boards require an additional USB interface (or a different board) to perform HID.
* USB HID libraries referenced in project (the sketch includes `USB.h`, `USBHIDKeyboard.h`, `USBHIDMouse.h`). Make sure the libraries installed match your board and core version.

---

## Quick setup & upload

1. Open the Arduino IDE.
2. Install the correct ESP32 board support (Espressif) via Boards Manager.
3. Install any missing libraries referenced by the project (`USB`, `USBHIDKeyboard`, `USBHIDMouse`) — consult the library manager or the sketch comments for exact library names.
4. Place the three files in a single sketch folder (e.g. `USB_Ware`) and open `latest_release.ino`.
5. Configure board and port: `Tools -> Board -> <your ESP32-S2/S3 board>` and select the appropriate COM port.
6. Edit `Configs.h` to change SSID, password, target folder, typing speed (`Delay`) and other settings if desired.
7. Compile and upload.
8. After the device boots it will host a Wi‑Fi AP (defaults to `USB Ware`). Connect to that AP and open the captive web UI (usually `http://10.0.0.1/` or the IP you configured).

---

## Configuration (important fields in `Configs.h`)

* `inputSSID` / `inputPASS` — Wi‑Fi AP SSID and password.
* `inputCH` — Wi‑Fi channel.
* `inputHidden` — whether AP is hidden.
* `maxClients` — maximum clients allowed.
* `MAC` — custom MAC address bytes.
* `local_IP`, `gateway`, `subnet` — network configuration.
* `Delay` — multiplier that controls typing speed. Increase to slow typing, decrease for faster.
* `linkURL` — URL opened by the Ping Site feature.
* `pymode` and `target` — mode (`e` encrypt / `d` decrypt) and target folder name used by the embedded Python demo.

---

## How the web UI triggers payloads

* The web UI (embedded in `Configs.h` as `MainPage[]`) posts parameters to `/set` on the ESP webserver.
* `latest_release.ino` handles `/set` and calls corresponding functions in `USB_Tools.h` to start USB HID and perform keystrokes.

---

## Payload details

* `py_script()` in `USB_Tools.h` types a PowerShell here‑string that writes a Python script `safe_dual_mode.py` to the user profile and executes it. The Python script demonstrates an encrypt/decrypt flow using the `cryptography` package.

  * The script attempts to install `cryptography` if it isn't present.
  * The script writes a generated key file and (in encrypt mode) will attempt to encrypt files under the specified target folder.
* `pingSite()` opens the URL in a browser.
* `barrelRollMode()` and `notepadMode()` are simple demos that open a search or Notepad and type text.

---

## Security & testing recommendations

* Test only on a VM or sacrificial test machine with no network shares and no valuable data.
* Prefer the `MODE = 'd'` (decrypt) only when you know the key file used for encryption exists, and avoid running `py_script()` on personal machines with important files.
* Use a small `Delay` value only on fast test hardware. If typing occurs too fast and the host misses keystrokes, increase `Delay`.
* Consider disabling or removing dangerous payloads before deploying to any environment.

---

## Troubleshooting

* **HID not working**: make sure your board supports native USB HID. ESP32-WROOM modules without native USB cannot emulate HID directly.
* **Compilation errors for USB libraries**: ensure the correct library versions for your core (ESP32-S2/S3 may require different libraries than AVR/Teensy boards).
* **Web UI unreachable**: verify AP is up and that your client got an IP in the configured range. Visit `http://10.0.0.1/` by default.

---

## License

```
MIT License
Copyright (c) 2025 dfyR433
Permission is hereby granted, free of charge, to any person obtaining a copy...
```
