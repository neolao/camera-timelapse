## ADDED Requirements

### Requirement: Wake-up and processing cycle
The system SHALL execute the following sequence on each wake-up: mount SD → read config → read/write counter → initialize camera → capture → save → unmount SD → deep sleep.

#### Scenario: Full nominal cycle
- **WHEN** all steps succeed
- **THEN** a photo is saved and deep sleep is triggered with the interval read from config.txt

### Requirement: Deep sleep via RTC timer
The system SHALL enter deep sleep after each cycle via `esp_sleep_enable_timer_wakeup()`. The duration SHALL match the `interval` configuration value (in microseconds).

#### Scenario: Normal deep sleep entry
- **WHEN** the cycle is complete (success or recoverable error)
- **THEN** the ESP32 enters deep sleep for the configured duration

### Requirement: Safety deep sleep on error
The system SHALL enter a 10-minute (600-second) deep sleep on any unrecoverable error (SD not mounted, camera not initialized, write failure) rather than staying in an active loop or restarting indefinitely.

#### Scenario: SD error at boot
- **WHEN** SD mount fails
- **THEN** an error message is emitted on the serial port and the ESP32 enters deep sleep for 600 seconds

#### Scenario: Camera error at boot
- **WHEN** camera initialization fails
- **THEN** an error message is emitted on the serial port and the ESP32 enters deep sleep for 600 seconds
