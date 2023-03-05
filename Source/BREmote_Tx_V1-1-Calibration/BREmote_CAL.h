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

#include "USER_CONFIG.h"

/*
 * Variables
 */
volatile bool power_enabled = 0;
 
int16_t meas_A0 = 0;
int16_t meas_A1 = 0;
int16_t meas_A2 = 0;
int16_t meas_A7 = 0;

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

#define EE_VAL_FLAG 3
#define EE_LEFTHANDER 4
#define EE_THR_MIN 5
#define EE_THR_MAX 7
#define EE_THR_REV 9
#define EE_TOG     10
#define EE_TOG_STB 12
#define EE_TOG_REV 14

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

#define HALL_EN A3

//MISC
#define MEAS_UB A6
#define EN_PWR 4
#define EN_MOT 5

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
