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

/*Init NRF Module */
void initNRF()
{
  DEBUG_PRINT("Enabling NRF...");
  
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
 
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.setRetries(2,1);

  radio.maskIRQ(1,1,0);

  radio.openWritingPipe(rxAddr);
  radio.openReadingPipe(1, txAddr);
  
  radio.writeAckPayload(1, &payload_arr, sizeof(payload_arr));
  radio.startListening();  // put radio in RX mode
  
  DEBUG_PRINTLN("Done");
}

//approx. 500us
void radioInterrupt()
{
  #ifdef SERVO_STEERING_ENABLED
    digitalWrite(EN_AUX1, LOW);
    delayMicroseconds(STEER_LOWEND + (rx_arr[3]<<STEER_MULTIPLIER));
    digitalWrite(EN_AUX1, HIGH);
  #else
    #ifdef DIFF_STEERING_ENABLED
      if(!failsafe)
      {
        digitalWrite(EN_AUX1, LOW);
        delayMicroseconds(secondThrottleTime);
        digitalWrite(EN_AUX1, HIGH);
      }
      else
      {
        digitalWrite(EN_AUX1, HIGH);
      }
    #else
      delayMicroseconds(500);
    #endif
  #endif
  bool tx_ds, tx_df, rx_dr;
  radio.whatHappened(tx_ds, tx_df, rx_dr);

  if(rx_dr)
  {      
    if (radio.available()) 
    {
      uint8_t bytes = radio.getDynamicPayloadSize();  // get the size of the payload
      radio.read(&rx_arr, sizeof(rx_arr));  // get incoming payload
      
      if(rx_arr[2] == 0xFF)
      {
        last_packet = millis();
        if(!failsafe)
        {
      	  #ifdef REVERSE_ENABLED
            int16_t ppmTime = (uint16_t)rx_arr[1] - 127;
            int16_t ppmTimeSecond = (uint16_t)rx_arr[1] - 127;
      	  #else
      	    int16_t ppmTime = (uint16_t)rx_arr[1];
            int16_t ppmTimeSecond = (uint16_t)rx_arr[1];
      	  #endif
          
          #ifdef DIFF_STEERING_ENABLED
            //Subtract from first motor
            if(rx_arr[3] > 127)
            {
              uint16_t ppmTimeModifier = ((uint16_t)rx_arr[3] - 127) * STEER_AMOUNT;
              if(ppmTime > ppmTimeModifier)
              {
                if(ppmTime >= 0)
                {
                  ppmTime -= ppmTimeModifier;
                }
                else
                {
                  ppmTimeSecond += ppmTimeModifier;
                }
              }
              else
              {
                ppmTime = 0;
              }
            }
            //Subtract from 2nd motor
            if(rx_arr[3] < 127)
            {
              uint16_t ppmTimeModifier = (127- (uint16_t)rx_arr[3]) * STEER_AMOUNT;
              if(ppmTimeSecond > ppmTimeModifier)
              {
                if(ppmTimeSecond >= 0)
                {
                  ppmTimeSecond -= ppmTimeModifier;
                }
                else
                {
                  ppmTime += ppmTimeModifier;
                }
              }
              else
              {
                ppmTimeSecond = 0;
              }
            }
            secondThrottleTime = (OCR1A_MID + (ppmTimeSecond<<OCR1A_MULTIPLIER))/2;
          #endif
          OCR1A = OCR1A_MID + (ppmTime<<OCR1A_MULTIPLIER);
        }
        radio.writeAckPayload(1, &payload_arr, sizeof(payload_arr));
      }      
    }
    #ifdef VESC_RELAY_MODE
      txdiv++;
      if(txdiv > 100)
      {
        txdiv = 0;
        delay(2);
        radio.stopListening();
        //radio.setRetries(1,1);
        radio.openWritingPipe(relayAddr);
        radio.write(&vescRelayBuffer, sizeof(vescRelayBuffer));
        delay(1);
        //radio.setRetries(5,2);
        radio.openWritingPipe(txAddr);
        radio.startListening();
      }
    #endif
  }
}

void updatePayload()
{
  uint16_t batConst = constrain(batVolt, FOIL_BAT_EMPTY_AT, FOIL_BAT_FULL_AT);
  payload_arr[2] = (uint8_t)map(batConst, FOIL_BAT_EMPTY_AT, FOIL_BAT_FULL_AT, 0, 99);
  payload_arr[4] = (uint8_t)constrain(fetTemp/10, 0, 99);
}
