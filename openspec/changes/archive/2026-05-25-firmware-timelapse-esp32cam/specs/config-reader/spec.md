## ADDED Requirements

### Requirement: Read config.txt
The system SHALL read the `config.txt` file at the SD root after a successful mount. The format SHALL be key=value, one entry per line. Blank lines and lines starting with `#` SHALL be ignored.

#### Scenario: File present and valid
- **WHEN** `config.txt` exists and contains valid entries
- **THEN** the read values are used for the current cycle

#### Scenario: File absent
- **WHEN** `config.txt` does not exist on the SD card
- **THEN** default values are used and an info log is emitted

### Requirement: interval parameter
The system SHALL read the `interval` key as an integer representing the deep sleep duration in seconds. The value SHALL be between 10 and 86400 (1 day). Default value: 300 (5 minutes).

#### Scenario: Valid interval
- **WHEN** `interval=600` is present in config.txt
- **THEN** the deep sleep lasts 600 seconds

#### Scenario: Out-of-range interval
- **WHEN** the interval value is less than 10 or greater than 86400
- **THEN** the default value (300) is used and a warning log is emitted

### Requirement: quality parameter
The system SHALL read the `quality` key as a JPEG integer from 0 (highest quality) to 63 (lowest quality). Default value: 10.

#### Scenario: Valid quality
- **WHEN** `quality=15` is present in config.txt
- **THEN** the camera is configured with quality=15

### Requirement: resolution parameter
The system SHALL read the `resolution` key as a string from: `UXGA`, `SXGA`, `XGA`, `SVGA`, `VGA`. Default value: `UXGA`.

#### Scenario: Valid resolution
- **WHEN** `resolution=SVGA` is present in config.txt
- **THEN** the camera captures at 800×600

#### Scenario: Unknown resolution
- **WHEN** the resolution value does not match any known value
- **THEN** `UXGA` is used and a warning log is emitted

### Requirement: flash parameter
The system SHALL read the `flash` key as one of: `off`, `on`, `auto`. Default value: `off`.
- `off`: flash LED never used
- `on`: flash LED always enabled during capture
- `auto`: flash LED enabled only if ambient light is insufficient (determined by AGC gain)

#### Scenario: Flash always on
- **WHEN** `flash=on` is present in config.txt
- **THEN** the flash LED is activated for every capture

#### Scenario: Flash auto — dark environment
- **WHEN** `flash=auto` and the sensor AGC gain is at or above `flash_threshold`
- **THEN** the flash LED is activated and a log line reports the gain and decision

#### Scenario: Flash auto — bright environment
- **WHEN** `flash=auto` and the sensor AGC gain is below `flash_threshold`
- **THEN** the flash LED is not activated

### Requirement: flash_threshold parameter
The system SHALL read the `flash_threshold` key as an integer from 0 to 30 representing the OV2640 AGC gain level above which auto flash is triggered. Default value: 15.

#### Scenario: Valid threshold
- **WHEN** `flash_threshold=20` is present in config.txt
- **THEN** auto flash triggers only when AGC gain ≥ 20
