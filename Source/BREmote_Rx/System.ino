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

/*Init Pins*/
void initPins()
{
  DEBUG_PRINT("Setting Pins...");

  pinMode(VESC_SERVO_INV, OUTPUT);
  pinMode(EN_WET_MEAS, OUTPUT);
  
  pinMode(NRF_IRQ, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(NRF_IRQ), radioInterrupt, FALLING);
  
  DEBUG_PRINTLN("Done");
}

void enableSerial()
{
  #ifdef DEBUG
  Serial.begin(9600);
  while (!Serial) //Wait for serial
  {
    delay(1);
    
  }
  #endif

  vescSerial.begin(115200);
}

void checkBMS()
{
  #ifdef BMS_DET_ACTIVE
    uint16_t bms_value = analogRead(BMS_MEAS);
    bms_value += analogRead(BMS_MEAS);
    if(bms_value < 300)
    {
      if(errorToSend == -1)
      {
        errorToSend = ERR_BMS;
      }
    }
    if(bms_value > 600)
    {
      if(errorToSend == ERR_BMS)
      {
        errorToSend = -1;
      }
    }
  #endif
}

void checkWetnessSensor()
{
  #ifdef WETNESS_DET_ACTIVE
    digitalWrite(EN_WET_MEAS, HIGH);
    uint16_t wetness = analogRead(WET_MEAS);
    wetness += analogRead(WET_MEAS);
    wetness += analogRead(WET_MEAS);
    wetness += analogRead(WET_MEAS);
    wetness += analogRead(WET_MEAS);
    wetness += analogRead(WET_MEAS);
    wetness += analogRead(WET_MEAS);
    wetness += analogRead(WET_MEAS);
    wetness /= 8;
    if(wetness < WETNESS_SENS - WETNESS_HYST)
    {
      if(errorToSend == -1)
      {
        errorToSend = ERR_WET;
      }
    }
    if(wetness > WETNESS_SENS + WETNESS_HYST)
    {
      if(errorToSend == ERR_WET)
      {
        errorToSend = -1;
      }
    }
  #endif
}

void setupTimer1()
{
  //Set MEGA328 Timer 1 Mode: (Page 109 Datasheet)
    //Timer Mode 14: FastPWM -> WGM11, 12, 13
    //Prescaler 8 -> CS11 (This allows best time-resolution (500ns/bit) while running at 61Hz or 122Hz
    //Genious calculator: https://www.ee-diary.com/p/atmega32-timer1-online-calculator.html
  
  #ifdef PWM_INVERTED
    //COM1A1 & COM1A0: Inverted Output on Pin9(PB1/OC1A)
    TCCR1A = (1<<COM1A1) + (1<<COM1A0) + (1<<WGM11);  
  #else
    //COM1A1: Non-Inverted Output on Pin9(PB1/OC1A)
    TCCR1A = (1<<COM1A1) + (1<<WGM11); 
  #endif
  
  TCCR1B = (1<<WGM13) + (1<<WGM12) + (1<<CS11);

  //This sets the PWM frequency: 0x7FFF -> 61Hz, 0x3FFF -> 122Hz
  ICR1 = 0x7FFF;

  //OCR1A is 0.5us (e.g. 2000 = 1000us)
  //By default, put out 1000us
  OCR1A = OCR1A_FAILSAFE;
  DDRB |= (1<<PB1);
}
