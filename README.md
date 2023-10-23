## PlayStation Reset Mod
In-Game Reset mod for PlayStation 1 (or One aka Slim).
Based on Arduino compatible boards. Arduino Pro Mini (ATmega328 or ATmega168) is recommended.

### Features
In-game reset using player one controller:
 - <kbd>SELECT + START + L2 + R2</kbd> - Reset
 - <kbd>SELECT + CROSS + L2 + R2</kbd> - Long Reset (useful for X-Station only)

### Building
Source code can be used by Arduino IDE or can be compiled using avr-toolchain.

### Configuration
Program behavior can be configured by defines that can be found in main.c file:
- `LONG_RESET_ENABLED` - long reset feature is enabled
- `USE_INTERRUPTS` - use interrupts for read controller data, instead of pure polling

### Installation
Here is a simplest connection pinout Arduino board to PlayStation board (without illustration):
| Arduino   |    | PlayStation board  |
|-----------|----|------------------|
|       5V  | <- | Pad Port Pin 5 (3.5V) or another +3.5V source point (VCC) |
|       GND | <- | GND |
| (PD2) D2  | <- | Pad Port Pin 7 (CLK) |
| (PD3) D3  | <- | Pad Port Pin 6 (ATT/SS) |
| (PD4) D4  | <- | Pad Port Pin 2 (CMD) |
| (PD5) D5  | <- | Pad Port Pin 1 (DAT) |
| (PD6) D6  | -> | RESET |
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

Pictures will be added soon.
