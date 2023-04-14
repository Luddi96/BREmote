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
  delayMicroseconds(250);
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
          uint16_t servoTime = (uint16_t)rx_arr[1];
          OCR1A = OCR1A_MID + (servoTime<<OCR1A_MULTIPLIER);
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
}
