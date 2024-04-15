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

/*
 * Includes
 */
 #include "SHARED_CONFIG.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SoftwareSerial.h>

#include "datatypes.h"
#include "buffer.h"
#include "crc.h"


/*
 * Variables
 */

// 0: Check to Remote, 1: Rem.Err Code, 2: Battery VESC, 3: 0xFF, 4: Temp from VESC
volatile uint8_t payload_arr[] = {0x0B, 0x00, 101, 0xFF, 101};
// 0: Check from Remote, 1: Trigger value, 2: 0xFF
volatile uint8_t rx_arr[4];
volatile uint8_t last_check = payload_arr[0];

volatile int errorToSend = -1;

volatile unsigned long last_packet = 0;
volatile uint8_t thr_received = 0;

volatile int16_t fetTemp = 0;
volatile int32_t motCur = 0;
volatile int32_t batCur = 0;
volatile int16_t duty = 0;
volatile int16_t batVolt = FOIL_BAT_FULL_AT;
volatile unsigned long get_vesc_timer = 0;
volatile unsigned long last_uart_packet = 0;

volatile bool failsafe = 1;

//#define VESC_MORE_VALUES
#ifdef VESC_MORE_VALUES
  #define VESC_PACK_LEN 19
  uint8_t vescRelayBuffer[25];
#else
  #define VESC_PACK_LEN 9
  uint8_t vescRelayBuffer[15];
#endif

//#define VESC_RELAY_MODE
#ifdef VESC_RELAY_MODE
  uint8_t txdiv = 0;
  uint8_t relayAddr[6] = {"BREAR"};
#endif

#ifdef DIFF_STEERING_ENABLED
volatile uint16_t secondThrottleTime = 0;
#endif


/*
 * Defines
 */
 
//PWM-Related:
#define PWM_INVERTED
//PWM-Timing-Related:
#define OCR1A_MID 3000

#ifdef REVERSE_ENABLED
  #define OCR1A_FAILSAFE 3000
  #define OCR1A_MULTIPLIER 3
#else
  #define OCR1A_FAILSAFE 2000
  #define OCR1A_MULTIPLIER 2
#endif

#define STEER_LOWEND 950
#define STEER_MULTIPLIER 2

//Diff steering
#define STEER_AMOUNT 1

//Analog

#ifdef SERVO_STEERING_ENABLED
  #define EN_AUX1 A1
  #define EN_AUX2 A2
#endif
#ifdef DIFF_STEERING_ENABLED
  #define EN_AUX1 A2
  #define EN_AUX2 A1
#endif

#define BMS_MEAS A0
#define EN_WET_MEAS A3
#define SDA A4
#define SCL A5
#define WET_MEAS A6
#define UBAT_MEAS A7

//SPI (NRF & ACC)
#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCK 13

#define NRF_CE 10
#define NRF_CSN 7
#define NRF_IRQ 2

#define ACC_CS 8
#define ACC_IRQ 3

#define EXT_CS 4

//UART2 to VESC
#define VESC_TX2 5
#define VESC_RX2 6

//Servo
#define VESC_SERVO_INV 9 //This must be Pin 9 or 10

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
