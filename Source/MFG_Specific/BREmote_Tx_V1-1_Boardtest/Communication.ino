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

void sendValues()
{
  //Write throttle value into array
  if(system_locked) tx_arr[1] = 0;
  else tx_arr[1] = thr_scaled;
  //Send
  if (radio.write(&tx_arr, sizeof(tx_arr))) 
  {
    //Is there ACK?
    if(radio.available())
    {
      radio.read(&rx_arr, sizeof(rx_arr));
      //Check if bit 3 is 0xFF
      if(rx_arr[3] == 0xFF)
      {
        //Copy check value to tx array, get opcode
        tx_arr[0] = rx_arr[0];
        opcode = rx_arr[1];
        vesc_battery = rx_arr[2];
        if(comm_errors >= 250) comm_errors = 249;
        if(comm_errors > 0) comm_errors--;
        last_activity = millis();
      }
      else
      {
        err_cause = ERR_PACKET_CONTENT;
        if(comm_errors <= 250) comm_errors++;
      }
    }
  }
  else
  {
    err_cause = ERR_NO_ACK;
    if(comm_errors <= 250) comm_errors++;
  }
}

/*Init NRF Module */
void initNRF()
{
  DEBUG_PRINT("Enabling NRF...");
  
  if(!radio.begin())
  {
    DEBUG_PRINT("Error NRF");
    CHAR1 = charset[LET_E];
    CHAR2 = charset[ERR_NRF];
    infiniteError();
  }
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
 
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.setRetries(2,1);

  radio.openWritingPipe(txAddr);
  radio.openReadingPipe(1, rxAddr);
  radio.stopListening();
  
  DEBUG_PRINTLN("Done");
}
