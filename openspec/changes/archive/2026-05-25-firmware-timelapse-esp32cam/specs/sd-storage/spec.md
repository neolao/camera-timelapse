## ADDED Requirements

### Requirement: SD card mount in SD_MMC 1-bit mode
The system SHALL mount the SD card via SD_MMC in 1-bit mode (CLK=GPIO14, CMD=GPIO15, D0=GPIO2) at each cycle.

#### Scenario: Successful mount
- **WHEN** a FAT32 SD card is present and mount is called
- **THEN** the card is accessible for read and write

#### Scenario: SD card absent or unreadable
- **WHEN** no SD card is detected or the mount fails
- **THEN** the firmware logs the error on the serial port and enters safety deep sleep

### Requirement: Persistent sequential counter
The system SHALL read `COUNTER.TXT` at the SD root to obtain the last used number. The counter SHALL be incremented and rewritten to `COUNTER.TXT` before the photo is captured. If `COUNTER.TXT` is absent or unreadable, the counter SHALL restart from 0.

#### Scenario: Existing counter
- **WHEN** `COUNTER.TXT` contains a valid integer N
- **THEN** the next photo is saved under number N+1 and `COUNTER.TXT` contains N+1

#### Scenario: Missing counter
- **WHEN** `COUNTER.TXT` does not exist
- **THEN** the first photo is saved under number 1 and `COUNTER.TXT` is created with value 1

#### Scenario: Corrupted counter
- **WHEN** `COUNTER.TXT` contains a non-integer value
- **THEN** the counter restarts from 0 and a warning log is emitted

### Requirement: JPEG photo save
The system SHALL save the captured photo in the `/photos` directory under the name `IMG_NNNNN.JPG` (N zero-padded to 5 digits). The `/photos` directory SHALL be created if it does not exist. The SD SHALL be cleanly unmounted after the write.

#### Scenario: Successful save
- **WHEN** the JPEG is captured and the counter is incremented
- **THEN** `/photos/IMG_NNNNN.JPG` is created on the SD with the full JPEG content

#### Scenario: Write error
- **WHEN** the file write fails (SD full, I/O error)
- **THEN** the firmware logs the error and enters safety deep sleep
