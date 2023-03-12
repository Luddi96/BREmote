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
  digitalWrite(HALL_EN, HIGH);
  
  analogReference(EXTERNAL);
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
}

void measMulti()
{
  meas_A0 = 0;
  meas_A1 = 0;
  meas_A2 = 0;
  meas_A7 = 0;

  analogReference(EXTERNAL);
  delay(100);
  analogRead(A0);
  analogRead(A0);
  delay(100);

  for(int i = 0; i < 10; i++)
  {
    meas_A0 += analogRead(A0);
    meas_A1 += analogRead(A1);
    delay(100);
  }
  meas_A0 /= 10;
  meas_A1 /= 10;

  analogReference(DEFAULT);
  delay(100);
  analogRead(A0);
  analogRead(A0);
  delay(100);

  for(int i = 0; i < 10; i++)
  {
    meas_A2 += analogRead(A2);
    meas_A7 += analogRead(A7);
    delay(100);
  }
  meas_A2 /= 10;
  meas_A7 /= 10;
}
