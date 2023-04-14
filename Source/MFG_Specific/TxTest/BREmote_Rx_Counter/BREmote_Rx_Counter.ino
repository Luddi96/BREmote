/* When flashing, select Processor: Aruino Nano (OLD BOOTLOADER)*/
/* When flashing, select Processor: Aruino Nano (OLD BOOTLOADER)*/
/* When flashing, select Processor: Aruino Nano (OLD BOOTLOADER)*/

/*
    BREmote Rx
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

#include "BREmote_Rx.h"

RF24 radio(NRF_CE, NRF_CSN); // CE, CSN
SoftwareSerial vescSerial(VESC_TX2,VESC_RX2); //Rx, Tx of ATMEGA

void setup() 
{
  enableSerial();
  initPins();
  initNRF();
  setupTimer1();
}

uint8_t dummy = 0;

void loop() 
{
  //If a VESC is connected via UART read from there
  #ifdef USE_VESC_UART
  //getVescLoop();
  #else
  //Else get voltage from analog input
  getUbatLoop();
  #endif
  

  // Check if there is a error to be transmitted to remote
  checkWetnessSensor();
  checkBMS();
  if(errorToSend != -1)
  {
    payload_arr[1] = errorToSend;
  }
  else
  {
    payload_arr[1] = 0;
  }

  Serial.println(millis() - last_packet);
  
  // Check for Failsafe
  if(millis() - last_packet > 2000)
  {
    failsafe = 1;
    OCR1A = OCR1A_FAILSAFE;
  }
  else
  {
    failsafe = 0;
  }
}
