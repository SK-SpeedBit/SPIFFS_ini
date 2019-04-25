/*
    ----------------------   SPIFFS_ini ver. 1.0  ----------------------
      (c) 2019 SpeedBit, reg. Czestochowa, Poland
    --------------------------------------------------------------------
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#ifndef _SPIFFS_ini_
#define _SPIFFS_ini_

#include "Arduino.h"
#include <string.h>

bool   ini_open (String ini_name);
String ini_read (String section, String key, String def);
bool   ini_write(String section, String key, String value);
bool   ini_close();

#endif // _SPIFFS_ini_
