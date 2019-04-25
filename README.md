# SPIFFS_ini

    A small module for handling ini files.
    Based on the SPIFFS file system.
    
    It works just like in Windows.
    Only four functions:
    - opening the ini file:     ini_open (String ini_name);
    - reading from ini file:    ini_read (String section, String key, String default);
    - writing to the ini file:  ini_write(String section, String key, String value);
    - closing the ini file:     ini_close();  
    
    It works on String variables.
    If you need to get a different type - just use the conversion.

    The readout time of a single variable is approx. 50 ms.
    The recording time is longer and is about 500ms.
    This is due to the way of recording, which is focused on saving memory, not time.

    Tested on ESP12E (1MB + 3MB SPIFFS)
