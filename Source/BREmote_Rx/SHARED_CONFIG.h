/*
    BREmote Shared Config File
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
 * This is the only file a user should make changes
 */
#error "Please change the adresses below to someting unique for your setup!! Do so both in Tx and Rx SHARED_CONFIG.h! Then delete this line"
//These Adresses need to consist of exactly 5 letters or numbers
uint8_t rxAddr[6] = {"BREAI"}; //From Rx To Tx (Board to Remote)
uint8_t txAddr[6] = {"BREAO"}; //From Tx To Rx (BRemote to Board)

//Output Debug Information
  #define DEBUG
  
  //#define SERVO_STEERING_ENABLED                    //Servo Output on Aux1 (Modification needed, email me)
  //#define DIFF_STEERING_ENABLED                     //2nd Motor Output on Aux1  (Modification needed, email me)
  //#define REVERSE_ENABLED                     //Puts remote into reverse mode: Only two "gears": Forward (F) and Reverse (P)
/*
 * Rx Specific
 */
    //#define USE_VESC_UART                 //If VESC is connected via UART
    //#define USE_UBAT_MEAS                 //If UBAT should be measured via Analog Input (will be overwritten by VESC)
    
    #define ANALOG_UBAT_FACTOR 0.853688     //Calibration factor for analog UBAT input, default: 0.025869
    
    //Foil Battery Mapping
      #define FOIL_BAT_EMPTY_AT 100         //10*V (e.g. 200 = 20V)
      #define FOIL_BAT_FULL_AT 600          //10*V (e.g. 200 = 20V)

    //Wetness Detection
      #define WETNESS_DET_ACTIVE            //If Wetness Detection should be active or not (generates E7)
      #define WETNESS_SENS 500              //Wetness sensitivity. 0: Least sensitive, 1024: Most sensitive, good range: 100 to 500
      #define WETNESS_HYST 50               //Wetness hysteresis. WETNESS_SENS - WETNESS_HYST must always be greater than 0, WETNESS_SENS + WETNESS_HYST must always be lower than 1024, good value: 50

    //#define BMS_DET_ACTIVE                //If External BMS Input should be read or not (generates E8)
/*
* Error Codes (don't change)
*/
#define ERR_PACKET_CONTENT 3
#define ERR_NO_ACK 4
#define ERR_EEPROM 5
#define ERR_NRF 6
#define ERR_WET 7
#define ERR_BMS 8
