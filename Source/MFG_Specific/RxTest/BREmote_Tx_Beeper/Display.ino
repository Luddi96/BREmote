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

void updateDisplay()
{
  if(display_enabled)
  {
      if(displaystate)
      {
        digitalWrite(C1, LOW);
        shiftOut(SR_DAT, SR_CLK, MSBFIRST, CHAR2 |(DOT2<<2));
        digitalWrite(SR_CLK, HIGH);
        digitalWrite(SR_CLK, LOW);
        digitalWrite(C2, HIGH);
      }
      else
      {
        digitalWrite(C2, LOW);
        shiftOut(SR_DAT, SR_CLK, MSBFIRST, CHAR1 |(DOT1<<2));
        digitalWrite(SR_CLK, HIGH);
        digitalWrite(SR_CLK, LOW);
        digitalWrite(C1, HIGH);
      }
  }
  else
  {
    digitalWrite(C1, LOW);
    digitalWrite(C2, LOW);
  }
}

void showNewGear()
{
  CHAR1 = charset[LET_L];
  CHAR2 = charset[gear-1];
}

void displayLock()
{
  CHAR1 = charset[LET_C];
  CHAR2 = 0x9A;
}

void blinkDotRoutine()
{
  if(blinkDot == 1)
  {
    DOT1 ^= 1;
  }
  else if(blinkDot == 2)
  {
    DOT2 ^= 1;
  }
  else if(blinkDot == 3)
  {
    DOT1 ^= 1;
    DOT2 = DOT1;
  }
  if(!(blinkDot & 0x01))
  {
    DOT1 = 0;
  }
  if(!(blinkDot & 0x02))
  {
    DOT2 = 0;
  }
}

void displayUU()
{
  CHAR1 = CHAR2 = charset[LET_U];
  unsigned long tim = millis();
  while(millis()-tim < 2000)
  {
    displaystate ^= 1;
    updateDisplay();
    delay(10);
  }
}

void displayBattery(int selection)
{
  //Internal
  if(selection == 1)
  {
    CHAR1 = charset[internal_battery/10];
    CHAR2 = charset[internal_battery%10];
  }
  //VESC
  if(selection == 2)
  {
    if(vesc_battery != 101)
    {
      uint8_t bat = constrain(vesc_battery,0,99);
      CHAR1 = charset[bat/10];
      CHAR2 = charset[bat%10];
    }
    else
    {
      CHAR1 = charset[DASH];
      CHAR2 = charset[DASH];
    }
  }
}

void displayError(int err)
{
  DOT1 = DOT2 = 0;
  CHAR1 = charset[LET_E];
  CHAR2 = charset[err];
}


void displayWelcomeMessage()
{
  CHAR1 = charset[LET_H];
  CHAR2 = charset[1];
}

void disableDisplay()
{
  display_enabled = 0;
  digitalWrite(C1, LOW);
  digitalWrite(C2, LOW);
}

void enableDisplay()
{
  display_enabled = 1;
}

#define ANIMATION_DELAY 100
void powerOnAnimation()
{
  CHAR1 = 0;
  CHAR2 = 0;
  delay(ANIMATION_DELAY);
  CHAR1 = 0x60;
  delay(ANIMATION_DELAY);
  CHAR1 = 0xF0;
  delay(ANIMATION_DELAY);
  CHAR2 = 0x90;
  delay(ANIMATION_DELAY);
  CHAR2 = 0x9A;
}

void powerOffAnimation()
{
  CHAR2 &= 0xF5;
  delay(ANIMATION_DELAY);
  CHAR2 &= 0x6E;
  delay(ANIMATION_DELAY);
  CHAR2 &= 0x9F;
  delay(ANIMATION_DELAY);

  CHAR1 &= 0xF5;
  delay(ANIMATION_DELAY);
  CHAR1 &= 0x6E;
  delay(ANIMATION_DELAY);
  CHAR1 &= 0x9F;
  delay(ANIMATION_DELAY);

/*  
  CHAR2 &= 0xF5;
  delay(ANIMATION_DELAY);
  CHAR2 &= 0x6F;
  delay(ANIMATION_DELAY);
  CHAR1 &= 0x6F;
  delay(ANIMATION_DELAY);
  CHAR1 &= 0x9F;*/
}
