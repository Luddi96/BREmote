# BREmote - Open Source eFoil and Esk8 remote 
![Banner](https://github.com/Luddi96/BREmote/blob/main/img/banner.png)

## Features:
* All mechanical parts 3D printed (even the springs)
* Symmetric design - left & right side can be swapped
* Sustainable - All external parts can be replaced
* Open Source: 3D Models, Electronics and Software are GPL3.0
* 2.4GHz Link (range dependant on antenna type, position)
* Communication with VESC via UART / CAN
* Low battery alarm (vibration)
* Water ingress alarm integrated in receiver
* Fall-off detection (not yet implemented)
* Gears / Power Levels / Cruise Control
* Charging and Programming via USB

## Links:
* [Promo Video](https://youtu.be/fF5BHrkHTNA)
* [Overview + Build Video](https://youtu.be/kb76z3FXLc0)
* [Deep Dive Video](https://youtu.be/cdQ3Ppyngpk)

## Parts needed:
### Electronics:
* [Tx PCB (files included in ZIP file)](https://jlcpcb.com/)
* [Rx PCB (files included in ZIP file)](https://jlcpcb.com/)
* [SMD NRF24L01 Module with onboard antenna](https://de.aliexpress.com/item/1005004905334589.html)
* [SMD NRF24L01 Module with external antenna](https://de.aliexpress.com/item/1005004905334589.html)
* [IPEX Antenna for NRF](https://de.aliexpress.com/item/1005003399563785.html)
* [0.36" 2-digit, 7-Segment Display, Common Cathode](https://de.aliexpress.com/item/1005003639616635.html)
* [2x SMD Header, 2x5-pin](https://de.aliexpress.com/item/32336498109.html)
* [EITHER Connector SP13 4-pin](https://de.aliexpress.com/item/1005001891060229.html)
* [OR Magnetic Connector](https://de.aliexpress.com/item/4001283688823.html)
* [Battery Sanyo UR14500AC 740mAh](https://www.nkon.nl/de/sanyo-ur14500ac-740mah-1-52a-reclaimed.html)
* [Vibration Motor 24x7mm, 3.7 to 5V](https://de.aliexpress.com/item/1005004063804397.html)
* [PCBs, order at JLCPCB, Gerbers see releases](https://github.com/Luddi96/BREmote/releases)
	* [Instruction how to order at JLC](https://youtu.be/y3GviDr0o1s)
### Mechanics:
* [2x Magnet N52 D3x6mm](https://www.ebay.de/itm/162903572491?var=461969545239)
* [2x Self-Threading Screw DIN7982 2.2x6.5mm](https://www.ebay.de/itm/272909463518?epid=17007662352)
* [PU Foam, ~Factor 15, e.g. Talamex 45729200](https://www.tradeinn.com/waveinn/en/talamex-pu-foam/138533717/p)
* [3D Printed Parts, see releases](https://github.com/Luddi96/BREmote/releases)

## Recommended Connector Pinning:
![Pinning1](https://github.com/Luddi96/BREmote/blob/main/img/pinning1.png) ![Pinning2](https://github.com/Luddi96/BREmote/blob/main/img/pinning2.png)

## Usage:
- With the remote off, pust the toggle forward to turn it on
- As long as you keep the toggle forward, you will be shown the internal battery capacity in %
- With `//#define NO_LOCK` disabled in SHARED_CONFIG.h:
	- The remote will start in locked state
	- Push the toggle down to unlock
	- The remote will show you "PP", demanding you to push the trigger to its maximum position
	- Once you release the trigger, the remote will be in operating state
	- Push the toggle up/down to increase/decrease gear (The remote will always start in *lowest* gear)
	- Push and hold the toggle down to lock again
	- Push and hold the toggle up to turn off the remote
- With `#define NO_LOCK` enabled in SHARED_CONFIG.h:
	- The remote will immediately start in unlocked state
	- Push the toggle up/down to increase/decrease gear  (The remote will always start in *highest* gear)
	- Push and hold the toggle up to turn off the remote

## Status Codes:
| Symbol | Meaning |
| --- | --- |
| [] |System Locked |
| PP |Press Trigger once to arm |
| L0-L9 |Current "Gear", use Toggle to change |
| UU |Undervoltage, charge Battery |

## Error Codes:
| Err Code | Meaning | Potential Causes |
| --- | --- | --- |
| E3 | Packet Content Invalid | Unexpected data received, probably due to SW error or signal interference |
| E4 | No Ack | No acknowledgement from Receiver, signal too weak or signal interference |
| E5 | EEPROM Error | Unexpected/no calibration in EEPROM, try calibrating the remote again |
| E6 | NRF Module | NRF (Wireless) Module can't be initialized, probably hardware fault |

---
# FAQ:
Q: When trying to program the remote with the Arduino IDE, I get an error "avrdude: stk500_getsync()"
A: Make sure you selected "Arduino Nano ATmega328P **(Old Bootloader)**"

---
# Changelog:

### 2023-03-12: [Release V1.2]
### 2023-03-12: [Source]
- Added more information, which Board and Processor to select for programming with Arduino IDE
- Renamed Source Files to not contain Version anymore
- Added Water Ingress Detection (Wetness Sensor) functionality
### 2023-03-07: [Release V1.1]
### 2023-03-04: [Electronics - Tx]
- Made Board 1mm smaller (each 0.5mm from top/bottom)
- Therefore shifted SPI vias and some other traces, changed style of motor pinheaders
### 2023-03-04: [Source]
- Found a error which depleated the battery in standby mode.
- Changed System.ino : disablePower() routine
### 2023-01-20: [Mechanics]
- Added Trigger Opitions with -15 and -30??
- Made Selector Rotator and Selector Spring thinner
- Updated "AllTheSmallThings"
### 2022-12-30: [Mechanics]
- Added Trigger spring with 1mm thickness (before: 1.2mm could lead to spring not being able to "roll in" all the way)
### 2022-12-02: [Electronics - Rx]
- Added protection diode to BMS measurement
### 2022-11-06: [Electronics - Tx]
- Better GND Connection for NRF Module
- Corrected Battery Polarity in whiteprint
- Added Diode to VREF
- Changed Reset to SMD Pad

---
# Credits:
Logo uses "watersport" and "Skate" by Adrien Coquet from https://thenounproject.com/. CC BY 3.0.

Thanks to Daniel for implementing and testing the magnetic connector!