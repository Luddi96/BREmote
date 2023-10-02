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

    //Behaviour of the User Interface
      //#define NO_LOCK             //No locking function, as soon as remote is on, throttle is active
      //#define NO_POWERBUTTON      //The toggle button cant turn off the remote (the remote will power off automatically if the receiver is off for > NO_ACTIVITY_TIMEOUT (see below))
      //#define NO_GEARS            //Gears can't be switched (set STARTGEAR to 9)
      
      #define STARTGEAR 0         //The gear that is set after poweron or unlock (0 to 9)

      #define SHOW_TEMP           //If display should also show temp (alternate btw. temp and bat) or only show bat
      
    // Hyteresis
      #define TOG_DIFF 8                          //Hysteresis for Toggle (30 for toto)
      #define TOG_STB_DIFF 8                      //Hysteresis for Standby Toggle (30 for toto)

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
      #define INT_VBAT_FULL_AT 410                //Voltage at which 100% internal Battery is displayed (100*V) DEFAULT 410
      #define INT_VBAT_EMPTY_AT 340               //Voltage at which 0% internal Battery is displayed (Also the protection limit), MIN: 34V (100*V) DEFAULT 340
      #define FOIL_VBAT_WARN_AT 10                //Percent at wich VESC Battery triggers a warning (%)
    
    //For Debug, Viration Motor can be disabled
      #define MOTOR_ENABLED


/*
* Error Codes (don't change)
*/
#define ERR_PACKET_CONTENT 3
#define ERR_NO_ACK 4
#define ERR_EEPROM 5
#define ERR_NRF 6
#define ERR_WET 7
#define ERR_BMS 8
