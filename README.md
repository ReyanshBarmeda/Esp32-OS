# 🖥️ ESP-OS Touch v3.5

[![Platform: ESP32](https://shields.io)](https://espressif.com)
[![Framework: Arduino](https://shields.io)](https://arduino.cc)
[![UI: LVGL v8](https://shields.io)](https://github.com)
[![License: MIT](https://shields.io)](LICENSE)

An interactive, graphical operating system built specifically for the base model **ESP32 (WROOM-32 / DevKitC)** featuring wireless file management, touchscreen interactions, and a real-time system performance dashboard.

---

## 📖 Project Overview

**ESP-OS Touch** is a lightweight, custom operating system designed to run on resource-constrained microcontrollers. Built directly on top of the native FreeRTOS kernel within the Espressif architecture, it replaces old text-based serial terminals with a beautiful, fully functional **Graphical User Interface (GUI)**. 

### ⚡ The 520 KB RAM Challenge
Because the base model ESP32 lacks external PSRAM, driving a full-color screen while running background networks usually crashes the chip. This project bypasses that limit using a **partial frame buffer rendering engine** (allocating only a 10-horizontal-line drawing memory window at a time). This keeps the RAM footprint safe while driving real-time graphics, touch inputs, and massive SD card directories seamlessly.

---

## 🚀 Core Features

* 🎨 **Visual Desktop Framework:** Built with **LVGL v8**, featuring modular layout banners, interactive navigation assets, and progress meters.
* 📂 **Wireless FTP Hard Drive Daemon:** Runs an active background FTP server. Wirelessly manage, upload, edit, or extract files directly from your PC using tools like FileZilla.
* 💾 **Shared SPI Bus Architecture:** Maximizes available hardware infrastructure pins by routing the TFT screen, touch controller, and MicroSD reader onto the exact same data highway.
* 📊 **Disk Footprint Analytics:** Recalculates real-time storage capacities and dynamically scales an on-screen graphical consumption bar.
* 🕒 **Atomic Web Time Clock:** Synchronizes over Wi-Fi with Network Time Protocol (NTP) servers to maintain accurate system clocks.
* 🗒️ **Live Kernel Logging Box:** A built-in terminal display frame showcasing system processes, boot parameters, and network event flags directly on the GUI screen.

---

## 📌 Hardware Wiring Schema

Both the LCD screen and the SD card reader are SPI peripherals. They share the same primary data lanes (`MOSI`, `MISO`, `SCK`) to save pins, but use unique **Chip Select (CS)** lines so the ESP32 can route signals cleanly.

| Peripheral Pin | ESP32 Pin | Connection Function |
| :--- | :--- | :--- |
| **VCC** | 5V / 3.3V | Microcontroller Power Rail |
| **GND** | GND | Power Ground Line |
| **MOSI** | **GPIO 23** | Shared SPI Master Out Data Highway |
| **MISO** | **GPIO 19** | Shared SPI Master In Data Highway |
| **SCK / CLK** | **GPIO 18** | Shared SPI Serial Clock Timing Line |
| **TFT_CS** | **GPIO 15** | Unique Screen Module Select |
| **TFT_DC** | **GPIO 2** | Screen Data / Command Toggle |
| **TFT_RST** | **GPIO 4** | Display Panel Hardware Reset |
| **SD_CS** | **GPIO 5** | Unique SD Card Module Select |
| **T_CS** | **GPIO 21** | Unique Touch Chip Controller Select |
| **T_IRQ** | **GPIO 22** | Touch Sensor Hardware Interrupt |

---

## 🛠️ Software Dependencies & Installation

To flash this project, ensure your Arduino IDE environment contains these libraries:

1. **SimpleFTPServer** (by Renzo Mischianti) - Installed via IDE Library Manager.
2. **lvgl** (by LVGL) - **IMPORTANT:** Install **Version 8.x** (e.g., v8.3.11). Do not use version 9.x as it breaks current syntax loops.
3. **TFT_eSPI** (by Bodmer) - Installed via IDE Library Manager.

### ⚠️ Library Calibration (Crucial Step)
Before compiling, you must tell the `TFT_eSPI` library what pins your display screen uses:
1. Navigate to your computer's storage path: `Documents/Arduino/libraries/TFT_eSPI/`
2. Open the file named **`User_Setup.h`** inside a text editor.
3. Uncomment the line matching your display driver (e.g., `#define ILI9341_DRIVER`).
4. Scroll down and edit the pin definitions to map exactly to the hardware table above:
   ```cpp
   #define TFT_MISO 19
   #define TFT_MOSI 23
   #define TFT_SCLK 18
   #define TFT_CS   15
   #define TFT_DC    2
   #define TFT_RST   4
   #define TOUCH_CS 21
   ```
5. Save the file and close it.

---

## 🎮 How To Run

1. Clone this repository to your computer.
2. Open the main `.ino` project file inside the Arduino IDE.
3. Update the `ssid` and `password` variables to match your home Wi-Fi details.
4. Select **ESP32 Dev Module** under your board manager settings.
5. Hit **Upload**.
6. Once online, open an FTP program (like **FileZilla**) on your PC, type the IP address shown on your screen, use username `admin` and password `password`, and start pushing storage files wirelessly to your ESP32's massive SD storage matrix!

---

## 📜 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

