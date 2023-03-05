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

/*
* This sketch can be used to monitor the VESC Parameters of a BREmote Receiver
* (with VESC_RELAY_MODE enabled), wirelessly (e.g. for friends outside of the water)
*/

#include <SPI.h>
#include "RF24.h"

#include "datatypes.h"
#include "buffer.h"
#include "crc.h"


RF24 radio(10,9);

byte addresses[][6] = {"BREAI","BREAR"};

float fetTemp = 0;
float motCur = 0;
float batCur = 0;
float duty = 0;
float batVolt = 0;

void setup() {
  Serial.begin(9600);
  
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
 
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.setRetries(2,1);

  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);

  radio.startListening();  // put radio in RX mode

  Serial.println("Tfet,Imot,Ibat,Duty,Ubat");
  Serial.println("Tfet,Imot,Ibat,Duty,Ubat");
}

uint8_t message[25];

void loop() 
{
  if( radio.available() )
  {
    uint8_t rxArr[25];
    radio.read( &rxArr, sizeof(rxArr));
    
    if(rxArr[0] == 2 && rxArr[rxArr[1]+4] == 3)
    {

      uint16_t crcMessage = 0;
      uint16_t crcPayload = 0;
  
      crcMessage = rxArr[rxArr[1]+2] << 8;
      crcMessage &= 0xFF00;
      crcMessage += rxArr[rxArr[1]+3];
  
      //Extract payload and also calculate CRC
      memcpy(&message,&rxArr[2],rxArr[1]);
  
      crcPayload = crc16(message, rxArr[1]);
      
      if(crcPayload == crcMessage)
      {
        int32_t cnt = 5;
        if(rxArr[1] == 19)
        {
          //Serial.println("Tfet,Imot,Ibat,Duty,Ubat");
          
          fetTemp = buffer_get_float16(message, 10.0, &cnt);
          motCur = buffer_get_float32(message, 100.0, &cnt);
          batCur = buffer_get_float32(message, 100.0, &cnt);
          duty = buffer_get_float16(message, 10.0, &cnt);
          batVolt = buffer_get_float16(message, 10.0, &cnt);

          //String sprt = "Fet Temp: " + String(fetTemp) + "°C, Mot. Curr.: " + String(motCur) + "A, Inp. Curr: " + String(batCur) + "A, Duty:: " + String(duty) + "%, Volt: " + String(batVolt) + "V";
          //String sprt = String(fetTemp) + "," + String(motCur) + "," + String(batCur) + "," + String(duty) + "," + String(batVolt);
          //Serial.println(sprt);
        }
        if(rxArr[1] == 9)
        {
          fetTemp = buffer_get_float16(message, 10.0, &cnt);
          batVolt = buffer_get_float16(message, 10.0, &cnt);

          //String sprt = "Fet Temp: " + String(fetTemp) + "°C, Volt: " + String(batVolt) + "V";
          //Serial.println(sprt);
        }
      }
      else
      {
        Serial.println("CRC NOK");
      }
    }
  }
  delay(100);

  String sprt = String(fetTemp) + "," + String(motCur) + "," + String(batCur) + "," + String(duty) + "," + String(batVolt);
  Serial.println(sprt);
}
