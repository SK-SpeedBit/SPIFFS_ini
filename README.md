# SPIFFS_ini

    A small module for handling ini files.
    It works just like in Windows.
    Only four functions:
    - opening the ini file:     ini_open (String ini_name);
    - reading from ini file:    ini_read (String section, String key, String def);
    - writing to the ini file:  ini_write(String section, String key, String value);
    - closing the ini file:     ini_close();  
    It works on String variables.
    If you need to get a different type - just use the conversion.

    The readout time of a single variable is approx. 50 ms.
    The recording time is longer and is about 500ms.
    This is due to the way of recording, which is focused on saving memory, not time.
