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

void sleepUntilMovement()
{
  digitalWrite(EN_MOT, LOW);
  mot_active = 0;
        
  #ifdef DEBUG
  DEBUG_PRINTLN("Going to sleep...");
  delay(200);
  Serial.end();
  #endif

  analogReference(DEFAULT);

  isr_active = 0;
  disablePower();
  disableDisplay();

  bool movementDetected = 0;
  while(!movementDetected)
  {
    //1 loop (from sleep to sleep) is approx. 220us
    //Sleep total average: 10uA

    digitalWrite(HALL_EN, LOW);
    ADCSRA &= ~(1 << 7); //Disable ADC
    
    //Actual sleep
    MCUCR |= (3 << 5); //set both BODS and BODSE at the same time
    MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
    __asm__  __volatile__("sleep");//in line assembler to go to sleep
    
    //Enable ADC & Stby Hall Sensors
    ADCSRA |= (1 << 7); //Enable ADC
    digitalWrite(HALL_EN, HIGH);
    delayMicroseconds(5); //Hall Sensor only needs 400ns to be accurate
    //Check if should wake up
    #ifdef NO_TOGGLE
    #else
      if(checkToggle(0))
      {
        if(checkToggle(1))
        {
          delay(200);
          if(checkToggle(1))
          {
            movementDetected = 1;
          }
        }
      }
    #endif
  }

  analogReference(EXTERNAL);
  enablePower();
  
  enableSerial();
  DEBUG_PRINTLN("Woke up...");

  intbat_filter = analogRead(MEAS_UB); //Discard first value
  delay(1);
  intbat_filter = analogRead(MEAS_UB); //Normally this is done in the ISR, which does not run yet
  
  calcInternalBattery();
  if(internal_battery < 10)
  {
    enableDisplay();
    displayUU();
    disableDisplay();
  }
  else
  { 
    initNRF();

    system_locked = 1;
    isr_active = 1;
    delay(150); //150ms = all Buffers are fully renewed
    enableDisplay();
    
    poweroff = 0;

    //Show internal Bat as long as toggle is held 
    displayBattery(1);
    delay(100);
    while((ctplus()))
    {
      delay(100);
    }
    blinkDot = 0;

    #ifdef NO_LOCK
      gear = 10;
      //Wait until throttle is free
      while(thr_scaled > 0)
      {
        CHAR1 = charset[LET_P];
        CHAR2 = charset[LET_P];
        delay(100);
      }
      CHAR1 = charset[LET_H];
      CHAR2 = charset[1];
      delay(500);
      system_locked = 0;
    #else
      gear = 1;
      powerOnAnimation();
      displayLock();
    #endif
    last_activity = millis();
  }
}
