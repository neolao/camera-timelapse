## Why

Monitor plant growth by capturing photos at regular intervals, fully autonomously, without any network infrastructure. The device (ESP32-CAM-MB / AI-Thinker) must operate on mains power or battery with minimal power consumption.

## What Changes

- Create an Arduino firmware (PlatformIO) for the AI-Thinker ESP32-CAM
- Capture a JPEG photo at UXGA resolution (1600×1200) on each wake-up
- Save to SD card with sequential naming that survives resets
- Configure the capture interval via a `config.txt` file on the SD card
- Deep sleep between each capture to minimize power consumption

## Capabilities

### New Capabilities

- `photo-capture`: Initialize the OV2640 camera and capture a JPEG at UXGA resolution
- `sd-storage`: Mount SD card (SD_MMC 1-bit), read/write the counter file, save photos
- `config-reader`: Read and parse `config.txt` from the SD card (interval, quality, resolution)
- `deep-sleep-cycle`: Wake-up, process, and return to deep sleep via RTC timer

### Modified Capabilities

## Impact

- New PlatformIO project — no existing code
- Dependencies: `espressif32` platform, `arduino` framework, `esp32-camera` and `SD_MMC` libraries (included in ESP32 Arduino core)
- SD card must be FAT32-formatted and contain a `config.txt` file
