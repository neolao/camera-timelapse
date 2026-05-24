# camera-timelapse

Autonomous timelapse firmware for the AI-Thinker ESP32-CAM-MB board. Captures JPEG photos at a configurable interval and saves them to a micro SD card. Designed for long-term, unattended operation (plant growth monitoring, construction sites, etc.).

## Hardware

- **Board**: AI-Thinker ESP32-CAM-MB (ESP32-CAM module + MB programmer board with CH340 USB)
- **Camera**: OV2640
- **Storage**: micro SD card (FAT32)

## Requirements

- [PlatformIO CLI](https://docs.platformio.org/en/latest/core/installation/index.html): `pip install platformio`
- Micro SD card formatted as FAT32

## SD Card Setup

Create a `config.txt` file at the root of the SD card:

```
interval=300
quality=10
resolution=UXGA
flash=off
flash_threshold=15
```

| Parameter | Values | Default | Description |
|---|---|---|---|
| `interval` | 10–86400 | 300 | Seconds between captures |
| `quality` | 0–63 | 10 | JPEG quality (0 = best, 63 = smallest) |
| `resolution` | `UXGA` (1600×1200) `SXGA` (1280×1024) `XGA` (1024×768) `SVGA` (800×600) `VGA` (640×480) | `UXGA` | Capture resolution |
| `flash` | `off` `on` `auto` | `off` | Flash LED mode |
| `flash_threshold` | 0–30 | 15 | AGC gain level above which auto flash triggers |

Photos are saved in a `photos/` folder on the SD card as `IMG_00001.JPG`, `IMG_00002.JPG`, etc. The sequence resumes correctly after a power cut or reset.

## Flashing

**Important**: remove the SD card before flashing.

Put the board in download mode: hold `IO0`, press `RST`, release `IO0`. Then:

```bash
pio run -t upload --upload-port /dev/ttyUSB0
```

After flashing, insert the SD card and press `RST` to start.

## Serial Monitor

```bash
pio device monitor --port /dev/ttyUSB0 --baud 115200
```

Logs are only visible during the active phase (~3–5 seconds per cycle). During deep sleep the serial connection is silent.

## Assembling the Timelapse Video

```bash
ffmpeg -framerate 24 -i photos/IMG_%05d.JPG -vcodec libx264 timelapse.mp4
```

## Power

The board operates on USB (5V). For battery operation, bypass the MB board and power the ESP32-CAM module directly at 3.3V — the CH340 chip on the MB board draws ~25 mA continuously and makes battery use impractical.
