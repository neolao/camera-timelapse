## ADDED Requirements

### Requirement: OV2640 camera initialization
The system SHALL initialize the OV2640 camera with the AI-Thinker pin mapping at the start of each cycle. The resolution SHALL be configurable (default: UXGA 1600×1200). The JPEG quality SHALL be configurable (default: 10).

#### Scenario: Successful initialization
- **WHEN** the firmware starts and calls the camera init function
- **THEN** the camera is ready to capture and no error is returned

#### Scenario: Initialization failure
- **WHEN** the camera does not respond during initialization
- **THEN** the firmware logs the error on the serial port and enters safety deep sleep

### Requirement: JPEG frame capture
The system SHALL capture a single JPEG frame after successful initialization. The image buffer SHALL be released after saving.

#### Scenario: Successful capture
- **WHEN** the camera is initialized and the capture function is called
- **THEN** a valid JPEG buffer (size > 0) is returned

#### Scenario: Failed capture
- **WHEN** the capture function returns a null or zero-size buffer
- **THEN** the firmware logs the error and enters safety deep sleep without attempting to write to the SD card
