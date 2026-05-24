#pragma once

// AI-Thinker ESP32-CAM camera pin mapping
#define CAM_PIN_PWDN    32
#define CAM_PIN_RESET   -1
#define CAM_PIN_XCLK     0
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27
#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39
#define CAM_PIN_D4      36
#define CAM_PIN_D3      21
#define CAM_PIN_D2      19
#define CAM_PIN_D1      18
#define CAM_PIN_D0       5
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22

// SD_MMC 1-bit mode pins (fixed by hardware, not configurable)
// CLK=GPIO14, CMD=GPIO15, D0=GPIO2

// Default configuration values
#define DEFAULT_INTERVAL_S   300
#define DEFAULT_QUALITY       10
#define DEFAULT_RESOLUTION   FRAMESIZE_UXGA

// Flash LED pin (GPIO 4 — free in SD_MMC 1-bit mode)
#define FLASH_PIN              4

// Safety deep sleep duration on unrecoverable error (seconds)
#define SAFETY_SLEEP_S       600

// Interval bounds (seconds)
#define INTERVAL_MIN_S        10
#define INTERVAL_MAX_S     86400

// Flash auto-mode AGC gain threshold (0–30); above this value = too dark → flash
#define DEFAULT_FLASH_THRESHOLD  15

// SD file names
#define COUNTER_FILE    "/COUNTER.TXT"
#define CONFIG_FILE     "/config.txt"
#define PHOTO_DIR       "/photos"
