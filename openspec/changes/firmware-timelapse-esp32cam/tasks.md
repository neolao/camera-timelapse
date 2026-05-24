## 1. PlatformIO Project Setup

- [ ] 1.1 Create `platformio.ini` with board `esp32cam`, framework `arduino`, serial speed 115200
- [ ] 1.2 Create `src/main.cpp` and `src/config.h` with constants (AI-Thinker pin mapping, default values)
- [ ] 1.3 Verify the project compiles without errors (`pio run`)

## 2. SD Configuration Reader

- [ ] 2.1 Implement SD_MMC 1-bit mount (GPIO14/15/2) with error handling
- [ ] 2.2 Implement `config.txt` parser (key=value, `#` comments, blank lines ignored)
- [ ] 2.3 Implement reading and validation of `interval`, `quality`, `resolution` parameters with fallback to defaults
- [ ] 2.4 Test on SD card with a valid `config.txt`, then missing, then malformed (verify serial logs)

## 3. Sequential Counter

- [ ] 3.1 Implement reading `COUNTER.TXT` (integer, returns 0 if absent or corrupted)
- [ ] 3.2 Implement writing `COUNTER.TXT` (N+1) before capture
- [ ] 3.3 Test correct counter resume after hardware reset (unplug/replug)

## 4. OV2640 Photo Capture

- [ ] 4.1 Implement camera initialization with AI-Thinker pin mapping
- [ ] 4.2 Apply resolution and quality read from config
- [ ] 4.3 Implement JPEG capture and buffer release after use
- [ ] 4.4 Test UXGA capture and verify JPEG integrity on PC

## 5. SD Photo Storage

- [ ] 5.1 Implement `IMG_NNNNN.JPG` save (zero-padded 5-digit number)
- [ ] 5.2 Implement clean SD unmount after write
- [ ] 5.3 Verify that successive cycles produce distinct, readable files

## 6. Deep Sleep Cycle

- [ ] 6.1 Implement `esp_sleep_enable_timer_wakeup()` with interval in microseconds
- [ ] 6.2 Implement safety deep sleep (600 s) on SD or camera error
- [ ] 6.3 Test a full cycle: wake-up → photo → sleep → automatic wake-up after interval
- [ ] 6.4 Verify serial output: logs must only appear during the active phase (~3 s)
