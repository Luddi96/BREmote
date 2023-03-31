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

void getVescLoop()
{
  if(millis() - get_vesc_timer > 1000)
  {
    // Get Voltage from VESC
    detachInterrupt(digitalPinToInterrupt(NRF_IRQ));
    if( getValuesSelective(&vescSerial) )
    {
      updatePayload();
      last_uart_packet = millis();
    }
    attachInterrupt(digitalPinToInterrupt(NRF_IRQ), radioInterrupt, FALLING);
    get_vesc_timer = millis();
  }

  // Check for VESC connection break
  if(millis() - last_uart_packet > 20000)
  {
    payload_arr[2] = 101;
  }
}

//total 4.5ms
bool getValuesSelective(Stream* interface)
{
  uint8_t vesc_command[5];
  vesc_command[0] = COMM_GET_VALUES_SELECTIVE;

  //Mask is 32 bit, divided in 4 byte, see "commands.c", line 377 in the VESC Firmware
  //Byte 4:
  #define FET_TEMP 0
  #define MotCurrent 2
  #define BatCurrent 3
  #define Duty 6
  //Byte 3:
  #define BatVolt 0

  vesc_command[1] = 0;
  vesc_command[2] = 0;
  vesc_command[3] = (1<<BatVolt);
  #ifdef VESC_MORE_VALUES
    vesc_command[4] = (1<<FET_TEMP)+ (1<<MotCurrent) + (1<<BatCurrent) + (1<<Duty);
  #else
    vesc_command[4] = (1<<FET_TEMP);
  #endif

  sendToVESC(vesc_command, 5, interface);

  //For the above values, answer should be 19 byte
  uint8_t message[25];
  
  //Is it really 9 or 19 byte?
  if(receiveFromVESC(message, interface) == VESC_PACK_LEN)
  {
    int32_t cnt = 5; //Dont care about the Mask    
    fetTemp = buffer_get_int16(message, &cnt);
    #ifdef VESC_MORE_VALUES
      motCur = buffer_get_int32(message, &cnt);
      batCur = buffer_get_int32(message, &cnt);
      duty = buffer_get_int16(message, &cnt);
    #endif
    batVolt = buffer_get_int16(message, &cnt);

    //String sprt = "Fet Temp: " + String(fetTemp) + "°C, Mot. Curr.: " + String(motCur) + "A, Inp. Curr: " + String(batCur) + "A, Duty:: " + String(duty) + "%, Volt: " + String(batVolt) + "V";
    //Serial.println(sprt);
    return 1;
  }
  else
  {
    return 0;
  }
}

int receiveFromVESC(uint8_t * buf, Stream* interface)
{
  uint8_t cnt = 0;
  uint8_t eom = 25;
  uint8_t raw_message[eom];
  bool rcv_err = 0;

  unsigned long started = millis();

  //Read until whole palyoad ist received
  while( ((millis() - started) < 200) && cnt != eom)
  {
    if(interface->available())
    {
      raw_message[cnt++] = interface->read();

      if(cnt == 1)
      {
        if(raw_message[0] != 2)
        {
          DEBUG_PRINTLN(".");
          cnt=0;
        }
      }
      if(cnt == 2)
      {
          eom = raw_message[1] + 5; //5 byte overhead
      }
    }
  }
  
/*
  Serial.println();
  Serial.println();

  for(int i = 0; i < 25; i++)
  {
    Serial.print(raw_message[i], HEX);
    Serial.print(" ");
  }

  Serial.println();
  Serial.println();
*/

  //Check if End is reached
  if(!rcv_err && raw_message[eom-1] == 3)
  {
    uint16_t crcMessage = 0;
    uint16_t crcPayload = 0;

    crcMessage = raw_message[eom - 3] << 8;
    crcMessage &= 0xFF00;
    crcMessage += raw_message[eom - 2];

    //Extract payload and also calculate CRC
    memcpy(buf,&raw_message[2],raw_message[1]);

    crcPayload = crc16(buf, raw_message[1]);

    if(crcPayload == crcMessage)
    {
      memcpy(&vescRelayBuffer[0], &raw_message[0], eom);
      return raw_message[1];
    }
    else
    {
      DEBUG_PRINTLN("CRC NOK");
      return 0;
    }
  }
  else
  {
    DEBUG_PRINTLN("Message Error");
    return 0;
  }
}

void sendToVESC(uint8_t * content, int len, Stream* interface)
{
  uint16_t crc = crc16(content, len);
  uint8_t tosend[16];
  int cnt = 0;

  tosend[cnt++] = 2;
  tosend[cnt++] = len;

  memcpy(tosend + cnt, content, len);

  cnt += len;

  tosend[cnt++] = (uint8_t)(crc >> 8);
  tosend[cnt++] = (uint8_t)(crc & 0xFF);
  tosend[cnt++] = 3;

  interface->write(tosend, cnt);
}
