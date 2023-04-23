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
* [Updated Build Video](https://youtu.be/RKn8jVGVD6g)
* [Deep Dive Video](https://youtu.be/cdQ3Ppyngpk)
* [Update Video 04/2023](https://youtu.be/SpBOiRbGKgA)
* [Steering & Long Range Update Video](https://youtu.be/DhPk98Wdcn0)

## Parts needed:
### Electronics:
* [Tx & Rx PCBs, order at JLCPCB, Gerbers see releases](https://github.com/Luddi96/BREmote/releases)
	* [Instruction how to order at JLC](https://youtu.be/y3GviDr0o1s)
* [Tx wireless module "Color: NRF24L01 SMD"](https://de.aliexpress.com/item/1005004905334589.html)
* [Rx wireless module "Color: GT-24 SMD"](https://de.aliexpress.com/item/1005004905334589.html)
* [IPEX Antenna for Rx](https://de.aliexpress.com/item/1005003399563785.html)
* [0.36" 2-digit, 7-Segment Display, Common Cathode "CC"](https://de.aliexpress.com/item/32730115504.html)
* [2x SMD Header, 2x5-pin](https://de.aliexpress.com/item/32336498109.html)
* [EITHER Connector SP13 4-pin](https://de.aliexpress.com/item/1005001891060229.html)
* [OR Magnetic Connector](https://de.aliexpress.com/item/4001283688823.html)
* [Battery Sanyo UR14500AC 740mAh](https://www.nkon.nl/de/sanyo-ur14500ac-740mah-1-52a-reclaimed.html)
* [Vibration Motor 24x7mm, 3.7 to 5V](https://de.aliexpress.com/item/1005004063804397.html)

### Mechanics:
* [2x Magnet N52 D3x6mm](https://www.ebay.de/itm/162903572491?var=461969545239)
* [2x Self-Threading Screw DIN7982 2.2x6.5mm](https://www.ebay.de/itm/272909463518?epid=17007662352)
* [PU Foam, ~Factor 15, e.g. Talamex 45729200](https://www.boatcareshop.nl/bootverf-bootlak/talamex-pu-schuim-2-componenten-200-cc)
* [3D Printed Parts, see releases](https://github.com/Luddi96/BREmote/releases)

## Recommended Connector Pinning:
![Pinning1](https://github.com/Luddi96/BREmote/blob/main/img/pinning1.png) ![Pinning2](https://github.com/Luddi96/BREmote/blob/main/img/pinning2.png)

## Usage:
- With the remote off, push the toggle forward to turn it on
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
| E7 | Water Ingress | Water on the water detection electrodes attached to the receiver. This feature can be disabled in code if you dont use it (default: enabled). |
| E8 | BMS Fault | BMS has disabled its output connected to the receiver. This feature can be disabled in code if you dont use it (default: disabled). |

# Connection Examples:

<details>
<summary>VESC with UART</summary>

![Conn1](https://github.com/Luddi96/BREmote/blob/main/img/conn1.png)

</details>

<details>
<summary>ESC or VESC WITHOUT UART</summary>

![Conn2](https://github.com/Luddi96/BREmote/blob/main/img/conn2.png)

</details>

---
# FAQ:
Q: When trying to program the remote with the Arduino IDE, I get an error "avrdude: stk500_getsync()"
A: Make sure you selected "Arduino Nano ATmega328P **(Old Bootloader)**"

---
# Changelog:

### 2023-04-23: [Release V1.5]
### 2023-04-23: [Source]
- Added Steering Function (see [update video](https://youtu.be/DhPk98Wdcn0) )
### 2023-04-23: [Electronics - Tx]
- Added longrange (LR) version of Tx board (uses same NRF module like Rx)
### 2023-04-14: [Release V1.4.2]
### 2023-04-14: [Source]
- Set MOSI Pin to "Input" in standby (Critical fix, as this could damage battery)
### 2023-04-14: [Source]
- Add compiler error if address is not changed
- Increase tolerances on some values
### 2023-04-14: [Release V1.4.1]
### 2023-04-14: [Source - Rx]
- Increased NRF interrupt-to-readout delay
### 2023-03-31: [Release V1.4]
### 2023-03-31: [Readme]
- Added connection diagrams
### 2023-03-31: [Source]
- Made connection to VESC UART optional (defined in SHARED_CONFIG)
- Added analog UBAT measurement
### 2023-03-31: [Electronics - Rx]
- Added component to make analog UBAT measurement more resitant to electromagnetic disturbance
### 2023-03-30: [Source]
- Add error timeout, to allow user to change gear during an error
### 2023-03-12: [Release V1.3]
### 2023-03-20: [Electronics - Rx]
- Added components to make Water Ingress Detection (Wetness Sensor) more resitant to electromagnetic disturbance
### 2023-03-20: [Source]
- Added BMS monitoring function
- Fixed Water Ingress Detection (Wetness Sensor) functionality
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
- Added Trigger Opitions with -15 and -30°
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