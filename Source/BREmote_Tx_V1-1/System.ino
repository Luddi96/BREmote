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

/* Set all registers for operation */
void initReg()
{
  DEBUG_PRINT("Setting register...");

  for(int i = 0; i < 10; i++)
  {
    thr_raw[i] = 0;
  }
  
  WDTCSR = (24); //change enable and WDE - also resets
  WDTCSR = (32); //prescalers only - get rid of the WDE and WDCE bit (32 = 4s, 33 = 8s)
  WDTCSR |= (1<<6); //enable interrupt mode
  
  //ENABLE SLEEP - this enables the sleep mode
  SMCR |= (1 << 2); //power down mode
  SMCR |= 1;//enable sleep
  DEBUG_PRINTLN("Done");
}

void initEEPROM()
{
  DEBUG_PRINT("Reading calib. from EEPROM...");
  if(EEPROM.read(EE_VAL_FLAG) == 0xBE)
  {
    if(EEPROM.read(EE_LEFTHANDER))
    {
      lefthander_ee = 1;
    }
    if(EEPROM.read(EE_THR_REV))
    {
      thr_rev_ee = 1;
    }
    if(EEPROM.read(EE_TOG_REV))
    {
      tog_rev_ee = 1;
    }
  
    thr_min_ee = ((uint16_t)EEPROM.read(EE_THR_MIN) << 8) + (uint16_t)EEPROM.read(EE_THR_MIN+1);
    thr_max_ee = ((uint16_t)EEPROM.read(EE_THR_MAX) << 8) + (uint16_t)EEPROM.read(EE_THR_MAX+1);
    tog_mid_ee = ((uint16_t)EEPROM.read(EE_TOG) << 8) + (uint16_t)EEPROM.read(EE_TOG+1);
    tod_mid_stb_ee = ((uint16_t)EEPROM.read(EE_TOG_STB) << 8) + (uint16_t)EEPROM.read(EE_TOG_STB+1);

    //Right Sensors: A0 and A2
    //Left Sensors: A1 and A7
    if(lefthander_ee)
    {
      pin_hall_thr = A0;
      pin_hall_tog = A1;
      pin_hall_tog_stb = A7;
    }
    else
    {
      pin_hall_thr = A1;
      pin_hall_tog = A0;
      pin_hall_tog_stb = A2;
    }
  }
  else
  {
    Serial.println("EEPROM Error");
    CHAR1 = charset[LET_E];
    CHAR2 = charset[ERR_EEPROM];
    infiniteError();
  }  
  DEBUG_PRINTLN("Done");
}

void infiniteError()
{
  enablePower();
  enableDisplay();
  while(1)
  {
    displaystate ^= 1;
    updateDisplay();
    delay(10);
  }
}

void calcInternalBattery()
{
  float vbat_raw = (float)intbat_filter * INT_VBAT_CALIBRATION;
  uint16_t vbat_const = constrain((uint16_t)(100.0*vbat_raw), INT_VBAT_EMPTY_AT, INT_VBAT_FULL_AT);

  internal_battery = map(vbat_const, INT_VBAT_EMPTY_AT, INT_VBAT_FULL_AT, 0, 99);
  internal_battery = constrain(internal_battery, 0, 99);
}

void checkLastActivity()
{
  if(millis()-last_activity > NO_ACTIVITY_TIMEOUT)
  {
    poweroff = 1;
  }
}

void enablePower()
{
  if(!power_enabled)
  {
    digitalWrite(EN_PWR, HIGH);
    delay(100);
    power_enabled = 1;
  }
}

void disablePower()
{
  digitalWrite(EN_PWR, LOW);
  power_enabled = 0;
  
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  digitalWrite(SPI_SCK, LOW);
  digitalWrite(SPI_MISO, LOW);
  digitalWrite(SPI_MOSI, LOW);
  digitalWrite(NRF_CSN, LOW);
  digitalWrite(NRF_CE, LOW);
  digitalWrite(NRF_IRQ, LOW);
  digitalWrite(ACC_CS, LOW);
  digitalWrite(ACC_IRQ, LOW);
  digitalWrite(SR_DAT, LOW);
  digitalWrite(SR_CLK, LOW);
}

/*Init Pins*/
void initPins()
{
  DEBUG_PRINT("Setting Pins...");
  pinMode(EN_PWR, OUTPUT);
  pinMode(EN_MOT, OUTPUT);
  pinMode(SR_DAT, OUTPUT);
  pinMode(SR_CLK, OUTPUT);
  pinMode(C1, OUTPUT);
  pinMode(C2, OUTPUT);
  pinMode(HALL_EN, OUTPUT);

  digitalWrite(EN_PWR, HIGH);
  digitalWrite(EN_MOT, LOW);
  digitalWrite(SR_DAT, LOW);
  digitalWrite(SR_CLK, LOW);
  digitalWrite(C1, LOW);
  digitalWrite(C2, LOW);
  digitalWrite(HALL_EN, LOW);  
  DEBUG_PRINTLN("Done");

  analogReference(EXTERNAL);
}

/*Timer 2 is for cyclic measuring and sending at 100Hz*/
void setupTimer2() 
{
  noInterrupts();
  // Clear registers
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  // 100.16 Hz (16000000/((155+1)*1024))
  OCR2A = 155;
  // CTC
  TCCR2A |= (1 << WGM21);
  // Prescaler 1024
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
  // Output Compare Match A Interrupt Enable
  TIMSK2 |= (1 << OCIE2A);
  interrupts();
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
}

void checkBattery()
{
  calcInternalBattery();
  if(internal_battery < 20)
  {
    blinkDot |= 0x01;
  }
  else
  {
    blinkDot &= 0xFE;
  }

  if(vesc_battery < VESC_VBAT_WARN_AT)
  {
    blinkDot |= 0x02;
  }
  else
  {
    blinkDot &= 0xFD;
  }
  
  if(internal_battery < 10)
  {
    poweroff = 1;
  }
}
