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

#include "BREmote_Tx.h"

RF24 radio(NRF_CE, NRF_CSN); // CE, CSN

void setup() 
{
  enableSerial();
  initReg();
  initPins();
  initNRF();
  initEEPROM();
  setupTimer2();
  enablePower();
  #ifdef NO_LOCK
    system_locked = 0;
    gear = 9;
  #endif
  delay(1000);
  enableDisplay();
}

void loop() 
{  
  Serial.println(millis()-last_activity);
  delay(10);
}

ISR(WDT_vect) 
{ //Leave empty
}

ISR(TIMER2_COMPA_vect)
{
  if(isr_active)
  {
    multidiv++;
    if(multidiv > 100)
    { 
      if(motordiv >= 10)
      {
        if(blinkDot)
        {
          #ifdef MOTOR_ENABLED 
            digitalWrite(EN_MOT, HIGH);
          #endif
          mot_active = 1;
          motordiv = 0;
        }
      }
      else
      {
        motordiv++;
        digitalWrite(EN_MOT, LOW);
        mot_active = 0;
      }
      //1Hz
      multidiv = 0;
      blinkDotRoutine();
    }
    //if(comm_errors >= 250 && multidiv % 10) sendValues();
    
    displaystate ^= 1;
    updateDisplay();
    measureAndBuffer();
    calcFilter();
    //if(comm_errors < 250) sendValues();
    sendValues();
  }
}
