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
  
  //#define STEERING_ENABLED                    //If steering feature is enable (see also fine tuning at "Tx Specific")

/*
 * Tx Specific
 */
 
    // No locking function, as soon as remote is on, throttle is active
      //#define NO_LOCK       
    
    // Hyteresis
      #define TOG_DIFF 8                          //Hysteresis for Toggle
      #define TOG_STB_DIFF 8                      //Hysteresis for Standby Toggle

    // Steering Feature (e.g. for tow boogies) - Enable at top of this file
      #define STEER_TOG_DIFF 15                   //Sensitivity of Steering
      #define STEER_DEADZONE 3                    //Deadzone for steering
      #define TOGGLE_BLOCK_TIME 500               //How long toogle button is in steering (*10ms)
    
    //Timings
      #define UNLOCK_TRIGGER_TIMEOUT 5000         //Time after unlock until trigger times out (ms)
      #define LOCK_PWR_WAITTIME 2000              //Time toggle needs to be pressed to power off or lock system (ms)
      
      #define GEAR_CHANGE_WAITTIME 100            //Time toggle needs to be pressed to change gear (ms)
      #define GEAR_DISPLAY_TIME 1000              //How long the new gear is shown (ms)

      #define ERR_DELETE_TIME 2000                //How long the "E-" is shown after deleting an error. In this time, the user can also change gear, even if the error is still persistent (and therefore will be shown again after this time is over)
    
      #define NO_ACTIVITY_TIMEOUT 60000           //Turn off power if no button is pressed and receiver has no connection (ms)
    
    //Internal Battery measurement
      #define INT_VBAT_CALIBRATION 0.004290224    //This converts ADC Bits to Voltage
      #define INT_VBAT_FULL_AT 410                //Voltage at which 1000% internal Battery is displayed (100*V) DEFAULT 410
      #define INT_VBAT_EMPTY_AT 340               //Voltage at which 0% internal Battery is displayed (Also the protection limit), MIN: 34V (100*V) DEFAULT 340
      #define FOIL_VBAT_WARN_AT 10                //Percent at wich VESC Battery triggers a warning (%)
    
    //For Debug, Viration Motor can be disabled
      #define MOTOR_ENABLED

/*
 * Rx Specific
 */
    #define USE_VESC_UART                 //If VESC is connected via UART
    
    #define ANALOG_UBAT_FACTOR 0.853688     //Calibration factor for analog UBAT input, default: 0.025869
    
    //Foil Battery Mapping
      #define FOIL_BAT_EMPTY_AT 180         //10*V (e.g. 200 = 20V)
      #define FOIL_BAT_FULL_AT 220          //10*V (e.g. 200 = 20V)

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
