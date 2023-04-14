/* When flashing fails, select Processor: Aruino Nano (OLD BOOTLOADER)*/
/* When flashing fails, select Processor: Aruino Nano (OLD BOOTLOADER)*/
/* When flashing fails, select Processor: Aruino Nano (OLD BOOTLOADER)*/

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

#include "BREmote.h"

RF24 radio(NRF_CE, NRF_CSN); // CE, CSN

void setup() 
{
  enableSerial();
  initPins();
  enablePower();
}

void loop() 
{

  if(EEPROM.read(EE_VAL_FLAG) == 0xBE)
  {
    uint16_t thr_min_read = 0;
    uint16_t thr_max_read = 0;
    uint16_t tog_read = 0;
    uint16_t tog_stb_read = 0;
    
    Serial.println("");
    Serial.println("Data in EEPROM:");
    
    thr_min_read = ((uint16_t)EEPROM.read(EE_THR_MIN) << 8) + (uint16_t)EEPROM.read(EE_THR_MIN+1);
    thr_max_read = ((uint16_t)EEPROM.read(EE_THR_MAX) << 8) + (uint16_t)EEPROM.read(EE_THR_MAX+1);
    tog_read = ((uint16_t)EEPROM.read(EE_TOG) << 8) + (uint16_t)EEPROM.read(EE_TOG+1);
    tog_stb_read = ((uint16_t)EEPROM.read(EE_TOG_STB) << 8) + (uint16_t)EEPROM.read(EE_TOG_STB+1);

    Serial.println("\n");
    if(EEPROM.read(EE_LEFTHANDER)) Serial.println("Lefthander, Trigger right");
    else Serial.println("Righthander, Trigger left");
    Serial.print("Min Throttle: ");
    Serial.println(thr_min_read);
    Serial.print("Max Throttle: ");
    Serial.println(thr_max_read);
    Serial.print("Throttle Reversed: ");
    Serial.println(EEPROM.read(EE_THR_REV));
    Serial.print("Idle Toggle: ");
    Serial.println(tog_read);
    Serial.print("Stb Toggle: ");
    Serial.println(tog_stb_read);
    Serial.print("Toggle Reversed: ");
    Serial.println(EEPROM.read(EE_TOG_REV));  
  }
  else
  {
    Serial.println("");
    Serial.println("EEPROM empty!");
  }
  
  Serial.println("");
  Serial.println("All Hands off, send charakter");

  while(!Serial.available()) delay (1);
  while(Serial.available()){ byte trash = Serial.read(); delay(100);}

  Serial.println("Measuring, dont move!");

  measMulti();

  int16_t meas_A0_before = meas_A0;
  int16_t meas_A1_before = meas_A1;
  int16_t meas_A2_before = meas_A2;
  int16_t meas_A7_before = meas_A7;

  Serial.println("Give full throttle, send charakter");

  while(!Serial.available()) delay (1);
  while(Serial.available()){ byte trash = Serial.read(); delay(100);}

  Serial.println("Measuring, dont move!");

  measMulti();

  int16_t diffA0 = abs(meas_A0_before-meas_A0);
  int16_t diffA1 = abs(meas_A1_before-meas_A1);

  int16_t thr_min = 0;
  int16_t thr_max = 0;

  int16_t toggle_idle = 0;
  int16_t toggle_idle_stb = 0;

  bool tog_rev = 0;
  bool thr_rev = 0;
  bool lefthander = 0;

  if(diffA0 > diffA1)
  {
    lefthander = 1;
    if(meas_A0_before > meas_A0)
    {
      thr_min = meas_A0;
      thr_max = meas_A0_before;
      thr_rev = 1;
    }
    else
    {
      thr_max = meas_A0;
      thr_min = meas_A0_before;
    }
    toggle_idle = meas_A1_before;
    toggle_idle_stb = meas_A7_before;
  }
  else
  {
    if(meas_A1_before > meas_A1)
    {
      thr_min = meas_A1;
      thr_max = meas_A1_before;
      thr_rev = 1;
    }
    else
    {
      thr_max = meas_A1;
      thr_min = meas_A1_before;
    }
    toggle_idle = meas_A0_before;
    toggle_idle_stb = meas_A2_before;
  }

  Serial.println("Push Toggle forward, send charakter");

  while(!Serial.available()) delay (1);
  while(Serial.available()){ byte trash = Serial.read(); delay(100);}

  Serial.println("Measuring, dont move!");

  meas_A0_before = meas_A0;
  meas_A1_before = meas_A1;

  measMulti();

  if(lefthander)
  {
    if(meas_A1_before > meas_A1)
    {
      tog_rev = 1;
    }
  }
  else
  {
    if(meas_A0_before > meas_A0)
    {
      tog_rev = 1;
    }
  }

  thr_min += 5;
  thr_max -= 5;

  Serial.println("\n");
  if(lefthander) Serial.println("Lefthander, Trigger right");
  else Serial.println("Righthander, Trigger left");
  Serial.print("Min Throttle: ");
  Serial.println(thr_min);
  Serial.print("Max Throttle: ");
  Serial.println(thr_max);
  Serial.print("Throttle Reversed: ");
  Serial.println(thr_rev);
  Serial.print("Idle Toggle: ");
  Serial.println(toggle_idle);
  Serial.print("Stb Toggle: ");
  Serial.println(toggle_idle_stb);
  Serial.print("Toggle Reversed: ");
  Serial.println(tog_rev);

  Serial.println("\nSaved to EEPROM");

  Serial.println("\n\n___________________________________\n!!!Flash final Software now, or Battery will be damaged!!!\n___________________________________");

  EEPROM.write(EE_LEFTHANDER, lefthander);
  EEPROM.write(EE_THR_MIN, thr_min >> 8);
  EEPROM.write(EE_THR_MIN+1, thr_min & 0xFF);
  EEPROM.write(EE_THR_MAX, thr_max >> 8);
  EEPROM.write(EE_THR_MAX+1, thr_max & 0xFF);
  EEPROM.write(EE_THR_REV, thr_rev);
  EEPROM.write(EE_TOG, toggle_idle >> 8);
  EEPROM.write(EE_TOG+1, toggle_idle & 0xFF);
  EEPROM.write(EE_TOG_STB, toggle_idle_stb >> 8);
  EEPROM.write(EE_TOG_STB+1, toggle_idle_stb & 0xFF);
  EEPROM.write(EE_TOG_REV, tog_rev);
  EEPROM.write(EE_VAL_FLAG, 0xBE);

  while(1);
}

ISR(WDT_vect) 
{ //Leave empty
}

ISR(TIMER2_COMPA_vect)
{/*
  if(isr_active)
  {
    multidiv++;
    if(multidiv > 100)
    {
      multidiv = 0;
    }
    
    displaystate ^= 1;
    updateDisplay();
    measureAndBuffer();
    calcFilter();
  }*/
}
