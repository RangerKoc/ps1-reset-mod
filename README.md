## PlayStation Reset Mod
In-Game Reset mod for PlayStation 1 and PSone (aka Slim, `yes, Slim model has reset point`).
Based on Arduino compatible boards. Arduino Pro Mini (ATmega328 or ATmega168) is recommended.

However, this project is obsolete, recommend using the [ps1-igr-mfo](https://github.com/RangerKoc/ps1-igr-mfo) mod, based on RP2040 (Pico, Zero, Tiny).

### Features
In-game reset using player one controller:
* <kbd>SELECT + START + L1 + R1</kbd> or <kbd>SELECT + START + L2 + R2</kbd> - Reset
* <kbd>SELECT + CROSS + L1 + R1</kbd> or <kbd>SELECT + CROSS + L2 + R2</kbd> - Long Reset (useful for X-Station only)

### Building and flashing
Arduino Nano: connect to PC using USB cable.
Arduino Pro Mini: use USB-UART module (e.g. CP2102 board with microUSB or Type-C), connect pins GND-GND, VCC-VCC, DTR-DTR, Tx-Rx, Rx-Tx, then connect module to PC using USB cable.

Using `Arduino IDE`:<br>
Open `ps1-reset-mod.ino` in Arduino IDE, select the appropriate Arduino Board type, select the appropriate COM port, then click `Upload` button.

Using `avr-toolchain`:<br>
Run `make` command in terminal, or run `build_atmega328.cmd` or `build_atmega168.cmd` command batch file.<br>
Then flash the hex file to Arduino Board using `avrdude` tool:<br>
```
avrdude -p atmega328 -c arduino -P COM3 -b 115200 -D -U flash:w:ps1-reset-mod.hex:i
```
or<br>
```
avrdude -p atmega168 -c arduino -P COM3 -b 115200 -D -U flash:w:ps1-reset-mod.hex:i
```

COM port number may be different on your system.<br>
Also COM port baudrate may vary depending of Arduino Board you using.
* ATmega328/ATmega168 optiboot bootloader: 115200
* ATmega328 old bootloader: 57600
* ATmega168 old bootloader: 19200

### Configuration
Program behavior can be configured by defines that can be found in `main.c` file:
- `LONG_RESET_ENABLED` - Long reset feature is enabled
- `USE_INTERRUPTS` - Use interrupts for read controller data, instead of pure polling

### Installation
Here is a simplest connection pinout Arduino board to PlayStation board:
| Arduino   |    | PlayStation board  |
|-----------|----|------------------|
|   5V/Vcc  | <- | Pad Port Pin 5 (3.5V) or another +3.5V source point (VCC) |
|       GND | <- | GND |
| (PD2) D2  | <- | Pad Port Pin 7 (CLK) |
| (PD3) D3  | <- | Pad Port Pin 6 (ATT/SS) |
| (PD4) D4  | <- | Pad Port Pin 1 (DAT) |
| (PD5) D5  | -- | `X` |
| (PD6) D6  | -> | RESET Pin in PSU connector or RESET point for PSone |
| (PD7) D7  | -- | `X` |
| (PB0) D8  | -- | `X` |
| (PB1) D9  | -- | `X` |
| (PB2) D10 | -- | `X` |
| (PB3) D11 | -- | `X` |
| (PB4) D12 | -- | `X` |
| (PC0) A0  | -- | `X` |
| (PC1) A1  | -- | `X` |
| (PC2) A2  | -- | `X` |
| (PC3) A3  | -- | `X` |
| (PC4) A4  | -- | `X` |
| (PC5) A5  | -- | `X` |

<p align="center"><img src="/guide/arduino-pins.png"/></p>

More pinouts can be found inside `guide` directory.

