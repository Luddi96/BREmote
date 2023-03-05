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
  detachInterrupt(digitalPinToInterrupt(NRF_IRQ));
  if( getValuesSelective(&vescSerial) )
  {
    updatePayload();
  }
  attachInterrupt(digitalPinToInterrupt(NRF_IRQ), radioInterrupt, FALLING);
  delay(1000);

  if(millis()-last_packet > 2000)
  {
    failsafe = 1;
    OCR1A = 2000;
  }
  else
  {
    failsafe = 0;
  }
}
