## Context

AI-Thinker ESP32-CAM-MB board: ESP32-CAM module (OV2640, SD slot) mounted on a Mother Board with a built-in CH340 USB-to-serial chip. No existing code — project created from scratch with PlatformIO + Arduino framework.

Known hardware constraints:
- GPIO 4 (flash LED) conflicts with SD in 4-bit mode → SD_MMC 1-bit is mandatory
- GPIO 0 = camera XCLK AND boot pin → handled automatically by the MB board
- The ESP32 has no battery-backed RTC → the photo counter must persist on the SD card

## Goals / Non-Goals

**Goals:**
- Autonomous firmware: boot → photo → sleep, no human intervention required
- Correct counter resume after any interruption (reset, power cut)
- Configurable capture interval without recompiling (file on SD card)
- Minimal power consumption to enable battery operation

**Non-Goals:**
- WiFi or Bluetooth connectivity (out of scope for v1)
- On-device video assembly
- Web interface or mobile application
- NTP time sync / file timestamping

## Decisions

### Framework: Arduino via PlatformIO CLI

Arduino simplifies access to the OV2640 and SD_MMC libraries already bundled in the ESP32 Arduino core. PlatformIO CLI enables compile, flash, and monitor from the command line without an IDE.

Alternative considered: native ESP-IDF → more control but unnecessary verbosity for this project.

### Sequential naming with counter persisted on SD

A `COUNTER.TXT` file at the SD root stores the last used number. On boot: read N, write N+1 immediately (before the photo), then save `IMG_NNNNN.JPG`.

Writing the counter before the photo: if capture fails, one number is lost but an existing photo is never overwritten.

Alternative considered: scan existing files to find the max → too slow with thousands of files.

### SD_MMC in 1-bit mode

Avoids the GPIO 4 / flash LED conflict. The reduced bandwidth is negligible for writing a single JPEG per cycle (a few hundred ms vs a few tens of ms).

### Deep sleep with RTC timer

`esp_sleep_enable_timer_wakeup()` + `esp_deep_sleep_start()`: ~10 µA consumption during sleep. With a 5-minute interval and an 18650 battery (3000 mAh), autonomy reaches several weeks.

Deep sleep cuts the USB serial connection — expected behavior. Logs are only visible during the active phase (~3 seconds per cycle).

### config.txt format

Simple key=value format, one entry per line:
```
interval=300
quality=10
resolution=UXGA
```
Manual parsing, no external library. Hardcoded defaults if the file is missing or malformed.

## Risks / Trade-offs

- **COUNTER.TXT corrupted** → Mitigation: if the file is unreadable, restart from 0 (photos may be overwritten). Acceptable for non-critical use.
- **SD not mounted** → Mitigation: serial log + safety deep sleep (10 min) to avoid an active busy loop.
- **OV2640 not initialized** → Mitigation: same, log + safety sleep.
- **GPIO 0 at boot**: if the SD card or a cable pulls GPIO 0 low, the ESP32 enters download mode. Mitigation: clean wiring, nothing connected to GPIO 0.
