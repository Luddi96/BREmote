/*
    BREmote V1.1
    Copyright (C) 2022 Ludwig Brechter (contact@lbre.de)
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SHARED_CONFIG.h"

/*
 * Variables
 */

//0: Return Check to Receiver, 1: Throttle Value, 2: 0xFF, 3: Steering
volatile uint8_t tx_arr[] = {0x00, 0x00, 0xFF, 0x00};
//0: Check from Receiver, 1: Rem.Err Code from Receiver, 2: Battery from VESC, 3: 0xFF, 4: Temp from VESC
uint8_t rx_arr[5];

volatile uint8_t remote_error = 0;
volatile bool remote_error_blocked = 0;
volatile uint8_t comm_errors = 0;
volatile int err_cause = 0;

volatile uint8_t vesc_battery = 101;
volatile uint8_t vesc_temp = 101;
volatile uint8_t internal_battery = 0;

//Ring Buffer for Hall Sensors
volatile uint16_t thr_raw[10];
volatile uint16_t tog_raw[10];
volatile uint16_t intbat_raw[10];
volatile uint16_t thr_filter = 0;
volatile uint16_t tog_filter = 0;
volatile uint16_t intbat_filter = 0;
volatile float thr_float = 0;
volatile uint8_t thr_scaled = 0;
volatile int filter_count = 0;

volatile float steer_float = 0;
volatile uint8_t steer_scaled = 0;

volatile bool isr_active = 1;

volatile uint8_t gear = STARTGEAR+1;

volatile bool system_locked = 1;
volatile bool poweroff = 0;

volatile bool power_enabled = 0;

volatile uint16_t toggle_blocked_counter = 0;
volatile bool toggle_blocked_by_steer = 0;

//Display Variables
volatile uint8_t CHAR1 = 0x00;
volatile uint8_t CHAR2 = 0x00;
volatile bool DOT1 = 0;
volatile bool DOT2 = 0;
volatile int blinkDot = 0;
volatile bool display_enabled = 0;
volatile bool displaystate = 0;

volatile uint8_t multidiv = 0;

volatile uint8_t motordiv = 0;
volatile bool mot_active = 0;

volatile unsigned long last_activity = 0;

uint16_t thr_min_ee = 0;
uint16_t thr_max_ee = 0;
uint16_t thr_min_stb_ee = 0;
uint16_t thr_max = 0;

uint16_t tog_mid_ee = 0;
uint16_t tog_mid_stb_ee = 0;

bool lefthander_ee = 0;
bool thr_rev_ee = 0;
bool tog_rev_ee = 0;

uint8_t pin_hall_thr = 0;
uint8_t pin_hall_tog = 0;
uint8_t pin_hall_thr_stb = 0;
uint8_t pin_hall_tog_stb = 0;

// softpower: when changing gear while fullthrottle, this damps the impact on new throttle
// debug output: BREmote_Tx.ino line 143ff

volatile uint16_t soft_power = 10;         // lower = harder impact, higher value = softer impact, 0 = no softpower, 10 = one step every 100ms
volatile int16_t soft_power_steps = 0;
volatile uint16_t soft_power_counter = 0;

/*
 * Includes
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <EEPROM.h>

/*
 * Defines
 */
 
//Hall Sensors:

#define HALL_EN A3

#define EE_VAL_FLAG 3
#define EE_LEFTHANDER 4
#define EE_THR_MIN 5
#define EE_THR_MAX 7
#define EE_THR_REV 9
#define EE_TOG     10
#define EE_TOG_STB 12
#define EE_TOG_REV 14
#define EE_THR_MIN_STB 16

//Display:
#define SR_DAT A4
#define SR_CLK A5
#define C1 7 //Changed here
#define C2 6 //Changed here

//SPI (NRF & ACC)
#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCK 13

#define NRF_CE 10
#define NRF_CSN 9
#define NRF_IRQ 2 

#define ACC_CS 8
#define ACC_IRQ 3

//MISC
#define MEAS_UB A6
#define EN_PWR 4
#define EN_MOT 5

/*
 * Display Stuff
 */

uint8_t charset[] = {0xfa, 0x0a, 0xb3, 0x9b, 0x4b, 0xd9, 0xf9, 0x1a, 0xfb, 0xdb, 0x7b, 0xf0, 0xf1, 0x71, 0x6b, 0xe0, 0x73, 0xea, 0x00, 0x01, 0xA1, 0xB1}; 
#define LET_A 10
#define LET_C 11
#define LET_E 12
#define LET_F 13
#define LET_H 14
#define LET_L 15
#define LET_P 16
#define LET_U 17
#define BLANK 18
#define DASH 19
#define LOWER_CELSIUS 20
#define UPPER_CELSIUS 21

/*
 * Debug
 */
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif
