#ifndef _SPIFFS_ini_
#define _SPIFFS_ini_

#include "Arduino.h"
#include <string.h>

bool   ini_open (String ini_name);
String ini_read_line();
bool   ini_eof();
String ini_read (String section, String key, String def);
bool   ini_write(String section, String key, String value);
bool   ini_close();

#endif // _SPIFFS_ini_
