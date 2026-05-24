## ADDED Requirements

### Requirement: OV2640 camera initialization
The system SHALL initialize the OV2640 camera with the AI-Thinker pin mapping at the start of each cycle. Before calling `esp_camera_init()`, the PWDN pin SHALL be cycled HIGH then LOW (100 ms each) to reset the sensor to a known state after deep sleep. The resolution SHALL be configurable (default: UXGA 1600×1200). The JPEG quality SHALL be configurable (default: 10).

#### Scenario: Successful initialization
- **WHEN** the firmware starts and calls the camera init function
- **THEN** the camera is ready to capture and no error is returned

#### Scenario: Initialization failure
- **WHEN** the camera does not respond during initialization
- **THEN** the firmware logs the error on the serial port and enters safety deep sleep

### Requirement: AEC stabilization
After initialization, the system SHALL discard 5 dummy frames (100 ms apart) to allow the OV2640 auto-exposure control (AEC) to stabilize before the real capture. If flash is active, 3 additional dummy frames SHALL be discarded after the flash LED is turned on.

#### Scenario: AEC warmup
- **WHEN** the camera is successfully initialized
- **THEN** 5 frames are captured and discarded before the photo is taken

#### Scenario: AEC warmup with flash
- **WHEN** flash is active (on or auto with gain ≥ threshold)
- **THEN** the flash LED is turned on, then 3 additional frames are discarded before capture

### Requirement: JPEG frame capture
The system SHALL capture a single JPEG frame after AEC stabilization. The image buffer SHALL be released after saving. The flash LED SHALL be turned off immediately after capture.

#### Scenario: Successful capture
- **WHEN** the camera is initialized and the capture function is called
- **THEN** a valid JPEG buffer (size > 0) is returned

#### Scenario: Failed capture
- **WHEN** the capture function returns a null or zero-size buffer
- **THEN** the firmware logs the error and enters safety deep sleep without attempting to write to the SD card
