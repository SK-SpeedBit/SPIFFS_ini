# SPIFFS_ini

<div class="pull-right">
  <img align="right" src="https://user-images.githubusercontent.com/33321284/56806055-224f1000-682b-11e9-9e86-b065917bcebc.png"/>
</div>

    A small module for handling ini files.
    Based on the SPIFFS file system.
   
    It works just like in Windows.
    Only four functions:
    - opening the ini file   : ini_open (String ini_name);
    - key reading            : ini_read (String section, String key, String default);
    - key writing            : ini_write(String section, String key, String value);
    - key deletion           : ini_delete_key(String section, String key);
    - section deletion       : ini_delete_section(String section);
    - closing the ini file   : ini_close();  
    
    It works on String variables.
    If you need to get a different type - just use the conversion.
    Comments inside ini file are allowed; (starting # or ;). Empty lines too.
    
    ver. 2.0
    Brand new version.
    Very fast action.
    All operations are performed in RAM!
    Writing to disk is only after calling ini_close ().
    Added procedures for deleting only the key or the entire section.
  
