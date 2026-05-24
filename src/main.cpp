#include <Arduino.h>
#include <SD_MMC.h>
#include "esp_camera.h"
#include "esp_sleep.h"
#include "config.h"

// ── Configuration ────────────────────────────────────────────────────────────

enum class FlashMode { OFF, ON, AUTO };

struct Config {
    uint32_t   intervalS      = DEFAULT_INTERVAL_S;
    uint8_t    quality        = DEFAULT_QUALITY;
    framesize_t resolution    = DEFAULT_RESOLUTION;
    FlashMode  flash          = FlashMode::OFF;
    uint8_t    flashThreshold = DEFAULT_FLASH_THRESHOLD;
};

static framesize_t parseResolution(const String& s) {
    if (s == "UXGA")  return FRAMESIZE_UXGA;
    if (s == "SXGA")  return FRAMESIZE_SXGA;
    if (s == "XGA")   return FRAMESIZE_XGA;
    if (s == "SVGA")  return FRAMESIZE_SVGA;
    if (s == "VGA")   return FRAMESIZE_VGA;
    Serial.println("[WARN] Unknown resolution, using UXGA");
    return FRAMESIZE_UXGA;
}

static Config readConfig() {
    Config cfg;
    File f = SD_MMC.open(CONFIG_FILE, FILE_READ);
    if (!f) {
        Serial.println("[INFO] config.txt not found, using defaults");
        return cfg;
    }
    while (f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();
        if (line.isEmpty() || line.startsWith("#")) continue;
        int sep = line.indexOf('=');
        if (sep < 0) continue;
        String key = line.substring(0, sep);
        String val = line.substring(sep + 1);
        key.trim();
        val.trim();
        if (key == "interval") {
            long v = val.toInt();
            if (v >= INTERVAL_MIN_S && v <= INTERVAL_MAX_S) {
                cfg.intervalS = (uint32_t)v;
            } else {
                Serial.printf("[WARN] interval %ld out of range, using default %d\n", v, DEFAULT_INTERVAL_S);
            }
        } else if (key == "quality") {
            cfg.quality = (uint8_t)constrain(val.toInt(), 0, 63);
        } else if (key == "resolution") {
            cfg.resolution = parseResolution(val);
        } else if (key == "flash") {
            if (val == "on")        cfg.flash = FlashMode::ON;
            else if (val == "auto") cfg.flash = FlashMode::AUTO;
            else                    cfg.flash = FlashMode::OFF;
        } else if (key == "flash_threshold") {
            cfg.flashThreshold = (uint8_t)constrain(val.toInt(), 0, 30);
        }
    }
    f.close();
    return cfg;
}

// ── Counter ──────────────────────────────────────────────────────────────────

static uint32_t readCounter() {
    File f = SD_MMC.open(COUNTER_FILE, FILE_READ);
    if (!f) return 0;
    String s = f.readStringUntil('\n');
    f.close();
    s.trim();
    long v = s.toInt();
    if (v <= 0 && s != "0") {
        Serial.println("[WARN] COUNTER.TXT corrupted, restarting from 0");
        return 0;
    }
    return (uint32_t)v;
}

static void writeCounter(uint32_t n) {
    File f = SD_MMC.open(COUNTER_FILE, FILE_WRITE);
    if (!f) {
        Serial.println("[ERROR] Cannot write COUNTER.TXT");
        return;
    }
    f.println(n);
    f.close();
}

// ── Camera ───────────────────────────────────────────────────────────────────

static bool initCamera(const Config& cfg) {
    // Cycle PWDN to reset the OV2640 to a known state after deep sleep
    pinMode(CAM_PIN_PWDN, OUTPUT);
    digitalWrite(CAM_PIN_PWDN, HIGH);
    delay(100);
    digitalWrite(CAM_PIN_PWDN, LOW);
    delay(100);

    camera_config_t camCfg = {};
    camCfg.ledc_channel = LEDC_CHANNEL_0;
    camCfg.ledc_timer   = LEDC_TIMER_0;
    camCfg.pin_d0       = CAM_PIN_D0;
    camCfg.pin_d1       = CAM_PIN_D1;
    camCfg.pin_d2       = CAM_PIN_D2;
    camCfg.pin_d3       = CAM_PIN_D3;
    camCfg.pin_d4       = CAM_PIN_D4;
    camCfg.pin_d5       = CAM_PIN_D5;
    camCfg.pin_d6       = CAM_PIN_D6;
    camCfg.pin_d7       = CAM_PIN_D7;
    camCfg.pin_xclk     = CAM_PIN_XCLK;
    camCfg.pin_pclk     = CAM_PIN_PCLK;
    camCfg.pin_vsync    = CAM_PIN_VSYNC;
    camCfg.pin_href     = CAM_PIN_HREF;
    camCfg.pin_sccb_sda = CAM_PIN_SIOD;
    camCfg.pin_sccb_scl = CAM_PIN_SIOC;
    camCfg.pin_pwdn     = CAM_PIN_PWDN;
    camCfg.pin_reset    = CAM_PIN_RESET;
    camCfg.xclk_freq_hz = 20000000;
    camCfg.pixel_format = PIXFORMAT_JPEG;
    camCfg.frame_size   = cfg.resolution;
    camCfg.jpeg_quality = cfg.quality;
    camCfg.fb_count     = 1;

    esp_err_t err = esp_camera_init(&camCfg);
    if (err != ESP_OK) {
        Serial.printf("[ERROR] Camera init failed: 0x%x\n", err);
        return false;
    }
    return true;
}

// ── Sleep helpers ─────────────────────────────────────────────────────────────

static void deepSleep(uint32_t seconds) {
    Serial.printf("[INFO] Sleeping for %u s\n", seconds);
    Serial.flush();
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_deep_sleep_start();
}

static void safetySleep() {
    Serial.println("[INFO] Entering safety sleep (600 s)");
    deepSleep(SAFETY_SLEEP_S);
}

// ── Main ─────────────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    Serial.println("[INFO] Wake-up");

    // Mount SD card (SD_MMC 1-bit mode)
    if (!SD_MMC.begin("/sdcard", true)) {
        Serial.println("[ERROR] SD mount failed");
        safetySleep();
    }

    // Read configuration
    Config cfg = readConfig();
    Serial.printf("[INFO] interval=%us quality=%u\n", cfg.intervalS, cfg.quality);

    // Read and increment counter (write before capture)
    uint32_t n = readCounter() + 1;
    writeCounter(n);

    // Initialize camera
    if (!initCamera(cfg)) {
        safetySleep();
    }

    // Discard a few frames to let AEC (auto-exposure) stabilize
    for (int i = 0; i < 5; i++) {
        camera_fb_t* dummy = esp_camera_fb_get();
        if (dummy) esp_camera_fb_return(dummy);
        delay(100);
    }

    // Determine whether to use the flash LED
    bool useFlash = false;
    if (cfg.flash == FlashMode::ON) {
        useFlash = true;
    } else if (cfg.flash == FlashMode::AUTO) {
        sensor_t* sensor = esp_camera_sensor_get();
        if (sensor) {
            // aec_value (0–1200) reflects actual auto-exposure; agc_gain is the manual register (always 0 in auto mode)
            int aec = sensor->status.aec_value;
            int aecThreshold = cfg.flashThreshold * 40; // map 0–30 → 0–1200
            useFlash = (aec >= aecThreshold);
            Serial.printf("[INFO] Auto flash: aec=%d threshold=%d flash=%s\n",
                          aec, aecThreshold, useFlash ? "on" : "off");
        }
    }

    if (useFlash) {
        pinMode(FLASH_PIN, OUTPUT);
        digitalWrite(FLASH_PIN, HIGH);
        // Let AEC re-settle with flash on
        for (int i = 0; i < 3; i++) {
            camera_fb_t* dummy = esp_camera_fb_get();
            if (dummy) esp_camera_fb_return(dummy);
            delay(100);
        }
    }

    // Capture photo
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb || fb->len == 0) {
        Serial.println("[ERROR] Capture failed");
        if (fb) esp_camera_fb_return(fb);
        safetySleep();
    }

    // Save photo
    SD_MMC.mkdir(PHOTO_DIR);
    char filename[32];
    snprintf(filename, sizeof(filename), PHOTO_DIR "/IMG_%05lu.JPG", (unsigned long)n);
    File photo = SD_MMC.open(filename, FILE_WRITE);
    if (!photo) {
        Serial.printf("[ERROR] Cannot open %s for writing\n", filename);
        esp_camera_fb_return(fb);
        safetySleep();
    }
    photo.write(fb->buf, fb->len);
    photo.close();
    esp_camera_fb_return(fb);

    if (useFlash) digitalWrite(FLASH_PIN, LOW);

    Serial.printf("[INFO] Saved %s (%u bytes)\n", filename, fb->len);

    // Unmount SD
    SD_MMC.end();

    // Deep sleep until next capture
    deepSleep(cfg.intervalS);
}

void loop() {
    // Never reached — deep sleep restarts via setup()
}
