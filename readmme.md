# ESP-OS Touch v3.5 🚀

An interactive, graphical operating system designed specifically for the base model **ESP32 (WROOM-32 / DevKitC)** featuring an interactive touchscreen UI, wireless file management, and a real-time system diagnostic dashboard.

---

## 📖 Project Overview

**ESP-OS Touch** is a custom, lightweight operating system built to run within highly resource-constrained environments. Operating directly on top of the native **FreeRTOS** kernel, it bypasses the base ESP32’s lack of external PSRAM by utilizing an optimized **partial frame-buffer rendering pipeline**. 

This memory-saving framework allows the system to drive a full-color desktop interface, capture live touch events, run background network infrastructure, and maintain massive storage pools simultaneously inside a strict **520 KB RAM boundary**.

---

## 🚀 Core Features

* 🎨 **Visual Desktop Workspace:** Powered by **LVGL v8**, offering real-time rendering of panels, interactive switches, applets, and status metrics.
* 📂 **Wireless FTP Daemon (Server):** Runs a background network listener that acts as your local cloud. Seamlessly drag-and-drop or transfer files wirelessly from any standard FTP client (like [FileZilla](https://filezilla-project.org/)).
* 💾 **Shared SPI Bus Architecture:** Links a high-resolution TFT LCD display and a high-capacity MicroSD card module onto the exact same physical data highway to maximize pin utility.
* 📊 **Live Storage Capacity Analytics:** A real-time disk utility applet that dynamically calculates SD storage space metrics and updates an on-screen graphical progress bar.
* 🕒 **Atomic Internet Clock Sync:** Automatically executes a background Network Time Protocol (NTP) handshake to fetch and display local system time accurately.
* 🗒️ **Live Kernel Diagnostic Log:** An integrated desktop console tracking ambient tasks and system events (such as active FTP handshakes or storage warnings).

---

## 🔧 Hardware Wiring Map

Because both the display and the card module are SPI peripherals, they share the core data highway pins. They are separated cleanly by assigning dedicated **Chip Select (CS)** lanes.

| Peripheral Pin | ESP32 Pin | Core System Role |
| :--- | :--- | :--- |
| **VCC** | 5V / 3.3V | Main Circuit Rail Power |
| **GND** | GND | Common Power Ground |
| **MOSI** | **GPIO 23** | Shared SPI Master Out Data Line |
| **MISO** | **GPIO 19** | Shared SPI Master In Data Line |
| **SCK / CLK** | **GPIO 18** | Shared SPI Serial Clock Line |
| **TFT_CS** | **GPIO 15** | Dedicated Screen Chip Select |
| **TFT_DC** | **GPIO 2** | Display Data / Command Toggle |
| **TFT_RST** | **GPIO 4** | Display Hardware Board Reset |
| **SD_CS** | **GPIO 5** | Dedicated SD Card Module Chip Select |
| **T_CS** | **GPIO 21** | Dedicated Touch Digitizer Chip Select |
| **T_IRQ** | **GPIO 22** | Touch System Hardware Interrupt Line |

---

## 💾 Software Installation & Setup

### 1. Required Libraries
Install the following dependencies directly inside your **Arduino IDE Library Manager**:
1. `TFT_eSPI` (by Bodmer)
2. `lvgl` (**Important: Install Version 8.x**, such as v8.3.11)
3. `SimpleFTPServer` (by Renzo Mischianti)

### 2. Display Configuration
Before flashing the code, navigate to your local libraries path:
`Documents/Arduino/libraries/TFT_eSPI/User_Setup.h`

Open this layout setup file inside a text editor, uncomment the line `#define ILI9341_DRIVER`, and ensure your pin maps match the exact pins defined in the table above.

---

## ⚙️ System Architecture Layer

```text
+-------------------------------------------------------+
|               Custom Interactive GUI                  |
|          (Buttons, Progress Bars, Text Logs)          |
+-------------------------------------------------------+
|        LVGL v8 Graphics Engine & Touch Driver         |
+-------------------------------------------------------+
|   SimpleFTPServer   |   Native SD   |   HTTP & Time   |
+-------------------------------------------------------+
|                 FreeRTOS Scheduler                    |
+-------------------------------------------------------+
|             ESP32 Hardware Core (520KB RAM)           |
+-------------------------------------------------------+
```

---

## 📝 License
This project is open-source and available under the [MIT License](https://opensource.org/licenses/MIT).