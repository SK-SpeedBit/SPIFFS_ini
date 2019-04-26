# SPIFFS_ini

<div class="pull-right">
  <img align="right" src="https://user-images.githubusercontent.com/33321284/56806055-224f1000-682b-11e9-9e86-b065917bcebc.png"/>
</div>

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
    Comments inside ini file are allowed; (starting # or ;). Empty lines too.
    
    The readout time of a single variable is approx. 50 ms.
    The recording time is longer and is about 500ms.
    
    This is due to the way of recording, which is focused on saving memory, not time.

    Tested on ESP12E (1MB + 3MB SPIFFS) CPU at 80MHz.
    Ini file approximately 1k, 80-100 lines.
