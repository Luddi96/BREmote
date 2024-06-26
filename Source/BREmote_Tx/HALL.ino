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

//Following block is the main user interface (plus, minus, on, lock, etc.)


void checkToggleButton()
{
  if(system_locked)
  {
    if(ctminus())
    {
      delay(300);
      if(ctminus())
      {
        //To unlock, promt user to touch throttle once
        CHAR1 = CHAR2 = charset[LET_P];
        unsigned long timeout = millis();
        while(thr_scaled < 20 && (millis()-timeout < UNLOCK_TRIGGER_TIMEOUT))
        {
          delay(100);
        }
        if(millis()-timeout < UNLOCK_TRIGGER_TIMEOUT)
        {
          CHAR1 = (1<<5 | 1<<7);
          CHAR2 = (1<<3 | 1<<7);
          delay(500);
          while(thr_scaled > 5)
          {
            delay(100);
          }
          gear = STARTGEAR+1;
          showNewGear();
          delay(1000);
          system_locked = 0;
        }
      }
      last_activity = millis();
    }
    else if(ctplus())
    {
      #ifndef NO_POWERBUTTON
      //In lock state, holding powers off
      delay(LOCK_PWR_WAITTIME);
      if(ctplus())
      {
        poweroff = 1;
      }
      last_activity = millis();
      #endif
    }
  }
  //System is NOT locked
  else
  {
    if(remote_error == 0 || remote_error_blocked == 1)
    {
      #ifdef STEERING_ENABLED
      if(!toggle_blocked_by_steer)
      #else
      if(1)
      #endif
      {
        if(ctminus())
        {
          delay(50);
          unsigned long pushtime = millis();
          bool decrease_once = 1;
          //minus decreases gear, holding locks system
          while(ctminus())
          {
            delay(10);
            if(millis() - pushtime > LOCK_PWR_WAITTIME)
            {
              if(thr_scaled < 10)
              {
                #ifndef NO_LOCK
                system_locked = 1;
                displayLock();
                last_activity = millis();
                #endif
              }
              while(ctminus())
              {
                delay(100);
              }
              break;
            }
            if(millis() - pushtime > GEAR_CHANGE_WAITTIME)
            {
              if(decrease_once)
              {
                #ifndef NO_GEARS
                if(gear > 1) {
                  gear --;
                  if (thr_float >= thr_max) soft_power_steps -= 25;       // if throttle is at max, soften gear change
                }
                showNewGear();
                decrease_once = 0;
                last_activity = millis();
                #endif
              }
            }
          }
          if(!(millis() - pushtime > LOCK_PWR_WAITTIME))
          {
            while(ctminus())
            {
              delay(10);
            }
            delay(50);
            while(millis() - pushtime < GEAR_DISPLAY_TIME)
            {
              checkToggleButton();
              delay(10);
            }
          }
        }
        else if(ctplus())
        {
          delay(50);
          unsigned long pushtime = millis();
          bool increase_once = 1;
          //Plus decreases gear, holding powers off
          while(ctplus())
          {
            delay(10);
            if(millis() - pushtime > LOCK_PWR_WAITTIME)
            {
              #ifndef NO_POWERBUTTON
              if(thr_scaled < 10) poweroff = 1;
              last_activity = millis();
              #endif
              break;
            }
            if(millis() - pushtime > GEAR_CHANGE_WAITTIME)
            {
              if(increase_once)
              {
                #ifndef NO_GEARS
                if(gear < MAX_GEARS) {
                  gear ++;
                  if (thr_float >= thr_max) soft_power_steps += 25;     // if throttle is at max, soften gear change
                }
                showNewGear();
                increase_once = 0;
                last_activity = millis();
                #endif
              }
            }
          }
          if(!(millis() - pushtime > LOCK_PWR_WAITTIME))
          {
            while(ctplus())
            {
              delay(10);
            }
            delay(50);
            while(millis() - pushtime < GEAR_DISPLAY_TIME)
            {
              checkToggleButton();
              delay(10);
            }
          }
        }
      }
    }
    else
    {
      if(ctminus() | ctplus())
      {
        delay(500);
        if(ctminus() | ctplus())
        {
          remote_error = 0;
          displayError(19);
          while(ctminus() | ctplus());
          remote_error_blocked = 1;
          unsigned long pushtime = millis();
          while(millis() - pushtime < ERR_DELETE_TIME)
          {
            checkToggleButton();
            delay(10);
          }
          remote_error_blocked = 0;
        }
      }
    }
  }
}

void measureAndBuffer()
{
  if(filter_count >= 10)
  {
    filter_count = 0;
  }
  
  thr_raw[filter_count] = analogRead(pin_hall_thr);
  tog_raw[filter_count] = analogRead(pin_hall_tog);
  if(!mot_active) intbat_raw[filter_count] = analogRead(MEAS_UB);
  
  filter_count++;
}

void calcFilter()
{
  thr_filter = tog_filter = intbat_filter = 0;
  for(int i = 0; i < 10; i++)
  {
    thr_filter += thr_raw[i];
    tog_filter += tog_raw[i];
    intbat_filter += intbat_raw[i];
  }
  
  thr_filter /= 10;
  tog_filter /= 10;
  intbat_filter /= 10;

  if(thr_rev_ee)
  {
    thr_float = (float)thr_max_ee - (float)thr_filter;
  }
  else
  {
    thr_float = (float)thr_filter - (float)thr_min_ee;
  }

  if (thr_float < thr_max) soft_power_steps = 0;      // if throttle is released from max, cancel softpower

  #ifdef STEERING_ENABLED                             // steering disables softpower
    soft_power_counter = 0;
  #endif

  #ifdef REVERSE_ENABLED
    uint16_t thr_tomap = (uint16_t)( constrain(thr_float,0.0,(float)thr_max_ee-thr_min_ee) * 5);
  #else
    uint16_t thr_tomap = (uint16_t)( constrain(thr_float,0.0,(float)thr_max_ee-thr_min_ee) * (float)gear);
  #endif
  thr_scaled = (uint8_t)map(thr_tomap, 0, (thr_max_ee-thr_min_ee)*10, 0, 255) - soft_power_steps;

  //Check if thr is touched
  if(thr_scaled > 10 && system_locked == 0)
  {
    //If so, block steer and reset counter
    toggle_blocked_by_steer = 1;
    toggle_blocked_counter = 0;
  }
  else
  {
    //If trigger was released, increment
    if(toggle_blocked_counter < TOGGLE_BLOCK_TIME)
    {
      toggle_blocked_counter ++;
    }
    //Until TOGGLE_BLOCK_TIME reached, then unlock toggle
    else
    {
      toggle_blocked_by_steer = 0;
    }
  }

  #ifdef STEERING_ENABLED
    if(tog_rev_ee)
    {
      steer_float = (float)tog_mid_ee - (float)tog_filter;
    }
    else
    {
      steer_float = (float)tog_filter - (float)tog_mid_ee;
    }
    
    if(toggle_blocked_by_steer)
    {
      if(steer_float > STEER_DEADZONE)
      {
        int16_t steer_deadzone = (int16_t)(constrain(steer_float-STEER_DEADZONE,0.0,STEER_TOG_DIFF-STEER_DEADZONE));
        steer_scaled = (uint8_t)map(steer_deadzone, 0, STEER_TOG_DIFF-STEER_DEADZONE,127,255);
      }
      else if (steer_float < -STEER_DEADZONE)
      {
        int16_t steer_deadzone = (int16_t)(constrain(steer_float+STEER_DEADZONE,-STEER_TOG_DIFF+STEER_DEADZONE,0.0));
        steer_scaled = (uint8_t)map(steer_deadzone, -STEER_TOG_DIFF+STEER_DEADZONE, 0,0,127);
      }
      else
      {
        steer_scaled = 127;
      } 
    }
    else
    {
      steer_scaled = 127;
    }
  #endif
}

bool checkToggle(bool precise)
{
  uint16_t tog = analogRead(pin_hall_tog_stb);
  if(precise)
  {
    delay(10);
    tog += analogRead(pin_hall_tog_stb);
    delay(10);
    tog += analogRead(pin_hall_tog_stb);

    tog /= 3;
  
    if(ctstbplus(tog)) return 1;
    else return 0;
  }
  else
  {
    if(ctstbplus(tog)) return 1;
    else return 0;
  }
}

bool checkThrottle()
{
  if(thr_rev_ee)
  {
    return(analogRead(pin_hall_thr_stb) < (thr_min_stb_ee - THR_STB_DIFF));
  }
  else
  {
    return(analogRead(pin_hall_thr_stb) > (thr_min_stb_ee + THR_STB_DIFF));
  }
}

bool ctplus()
{
  if(tog_rev_ee)
  {
    return((tog_filter < tog_mid_ee - TOG_DIFF));
  }
  else
  {
    return((tog_filter > tog_mid_ee + TOG_DIFF));
  }
}

bool ctminus()
{
  if(tog_rev_ee)
  {
    return((tog_filter > tog_mid_ee + TOG_DIFF));
  }
  else
  {
    return((tog_filter < tog_mid_ee - TOG_DIFF));
  }
}

bool ctstbplus(uint16_t tog)
{
  if(tog_rev_ee)
  {
    return((tog < tog_mid_stb_ee - TOG_STB_DIFF));
  }
  else
  {
    return((tog > tog_mid_stb_ee + TOG_STB_DIFF));
  }
}

bool ctstbminus(uint16_t tog)
{
  if(tog_rev_ee)
  {
    return((tog > tog_mid_stb_ee + TOG_STB_DIFF));
  }
  else
  {
    return((tog < tog_mid_stb_ee - TOG_STB_DIFF));
  }
}
