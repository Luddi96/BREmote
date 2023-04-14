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

#ifdef NO_LOCK
  void checkToggleButton()
  {
    if(remote_error == 0 || remote_error_blocked == 1)
    {
      if(ctminus())
      {
        delay(50);
        unsigned long pushtime = millis();
        //Minus only decreases gear
        while(ctminus())
        {
          delay(10);
          if(millis() - pushtime > GEAR_CHANGE_WAITTIME)
          {
            break;
          }
        }
        if (millis() - pushtime > GEAR_CHANGE_WAITTIME)
        {
          if(gear > 1) gear --;
          showNewGear();
        }
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
        last_activity = millis();
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
            poweroff = 1;
            break;
          }
          if(millis() - pushtime > GEAR_CHANGE_WAITTIME)
          {
            if(increase_once)
            {
              if(gear < 10) gear ++;
              showNewGear();
              increase_once = 0;
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
        last_activity = millis();
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
#else
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
            gear = 1;
            showNewGear();
            delay(1000);
            system_locked = 0;
          }
        }
        last_activity = millis();
      }
      else if(ctplus())
      {
        //In lock state, holding powers off
        delay(LOCK_PWR_WAITTIME);
        if(ctplus())
        {
          poweroff = 1;
        }
        last_activity = millis();
      }
    }
    //System is NOT locked
    else
    {
      if(remote_error == 0 || remote_error_blocked == 1)
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
              system_locked = 1;
              displayLock();
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
                if(gear > 1) gear --;
                showNewGear();
                decrease_once = 0;
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
          last_activity = millis();
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
              poweroff = 1;
              break;
            }
            if(millis() - pushtime > GEAR_CHANGE_WAITTIME)
            {
              if(increase_once)
              {
                if(gear < 10) gear ++;
                showNewGear();
                increase_once = 0;
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
          last_activity = millis();
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
#endif

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

  uint16_t thr_tomap = (uint16_t)(constrain(thr_float,0.0,(float)thr_max_ee-thr_min_ee)*(float)gear);
  thr_scaled = (uint8_t)map(thr_tomap, 0, (thr_max_ee-thr_min_ee)*10, 0, 255);
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
    return((tog < tod_mid_stb_ee - TOG_STB_DIFF));
  }
  else
  {
    return((tog > tod_mid_stb_ee + TOG_STB_DIFF));
  }
}

bool ctstbminus(uint16_t tog)
{
  if(tog_rev_ee)
  {
    return((tog > tod_mid_stb_ee + TOG_STB_DIFF));
  }
  else
  {
    return((tog < tod_mid_stb_ee - TOG_STB_DIFF));
  }
}
