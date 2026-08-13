# Cargo Cult Multi-Adv

Cargo Cult Multi-Adv emulates roles for the Cult of Cthulhu DEF CON 34 indie badge, letting badge owners continue and complete their badge experience after DEF CON 34 on its C3-validated target; other compatible multi-advertising ESP32 targets are untested.

Cargo Cult Multi-Adv is a C3-focused Cargo Cult role emulator that supports
concurrent BLE advertisements. It is intended for boards using the
`esp32-c3-devkitm-1` PlatformIO environment.

## Supported hardware

`ESP32-C3` is the validated target for this repository. Other ESP32 chips that
support compatible concurrent multi-advertising may work, but they are
untested. Use the C3 environment below for the supported configuration.

## Build and flash

Install PlatformIO, connect the board, then build and upload with:

```text
py -m platformio run -e esp32-c3-devkitm-1 -t upload --upload-port <serial-port>
```

Replace `<serial-port>` with the port assigned to the connected board by your
operating system. The command targets the board configuration in
`platformio.ini`; it does not require a board-specific fork of this guide.

## C3 reference wiring

The validated C3 reference wiring uses an I2C OLED and two status LEDs:

| Function | Pin |
| --- | --- |
| OLED SDA | GPIO 5 |
| OLED SCL | GPIO 6 |
| Green LED | GPIO 3 |
| Red LED | GPIO 4 |

Use these GPIO assignments when reproducing the validated C3 setup. The OLED
is addressed at `0x3C` by the firmware.

## Serial control

Open the serial monitor at `115200` baud. Commands are line-oriented and are
case-insensitive:

```text
help
status
mode auto
mode manual
mode seance
role acolyte
role glyph
role elder
role cthulhu
next
```

Automatic mode rotates roles. `mode seance` starts the Seance phase directly;
`status` reports the active mode and advertisement state.

## Compatibility

| Repository | Target | Multi-advertising behavior | Seance behavior |
| --- | --- | --- | --- |
| This repository | ESP32-C3 | Three simultaneous Acolyte advertisements | Three simultaneous Seance peers |
| `cargo-cult` | Classic ESP32/WROOM | Original single-role behavior | No functional Seance |

This repository is deliberately separate from `cargo-cult`: `cargo-cult` is
for classic ESP32/WROOM hardware and has no functional Seance. Cargo Cult
Multi-Adv uses the C3 configuration to advertise three simultaneous Acolyte
identities or three simultaneous Seance peers.
