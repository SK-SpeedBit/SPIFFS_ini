

#include <SPIFFS_ini.h>
#include "FS.h"


File ini;

String ini_read_line() {
  if (!ini) return "";

  String s = "";
  char c = 0;
  bool test = false;

  do { // read string until CR or LF
    if ( ini_eof() ) break;
    c = ini.read();
    test = (c != 13) && (c != 10);
    if (test) s +=  c;
  } while (test);

  do { // read all CR and LF (empty lines too)
    if ( ini_eof() ) break;
    c = ini.peek();
    test = (c == 13) || (c == 10);
    if (test) ini.read();
  } while (test);

  s.trim(); // trim string

  return s;
}


bool ini_open (String ini_name) {
  if (ini) ini_close();

  if (SPIFFS.exists(ini_name))
    ini = SPIFFS.open(ini_name, "r");

  return ini;
}


bool ini_eof() {
  if (!ini) return true;
  return (ini.available() <= 0);
}


String ini_read(String section, String key, String def) {
  if (!ini) return "";

  section.toUpperCase();
  key.toUpperCase();

  String s = "";
  String k = "";
  String v = "";
  bool found = false;
  int ind = 0;

  ini.seek(0);
  while (!ini_eof() ) { // find section
    s = ini_read_line();
    if ( (s.charAt(0)=='#') || (s.charAt(0)==';') ) continue; // comment - get next line
    s.toUpperCase();
    if ( (s.charAt(0)=='[') && (s.charAt(s.length()-1)==']') ) { // it's section
      s.remove(0, 1);
      s.remove( (s.length()-1), 1);
      if ( s == section) { found = true; break; } // got it!
    }
  }
  if (found) {
    found = false;
    while (!ini_eof() ) { // find key
      s = ini_read_line();
      if ( (s.charAt(0)=='#') || (s.charAt(0)==';') ) continue; // comment - get next line
      s.toUpperCase();
      if ( (s.charAt(0)!='[') && (s.charAt(s.length()-1)!=']') ) {  // it is not next section...
        ind = s.indexOf('=');
        if (ind>=0) { // found "="
          k = s.substring(0, ind);
          v = s.substring(ind + 1);
          k.trim(); // trim key string
          v.trim(); // trim value string
          k.toUpperCase();
          if (k == key) { // got key
            found = true;
            break;
          }
        }
      }
      else break; // got next section - break
    }
  }
  if (found) return v;
  else return def;
}


//bool ini_write(String section, String key, String value) {
String ini_write(String section, String key, String value) {
  //if (!ini) return false;
  if (!ini) return "";

  File tmpini;
  const String tmpname = "tmpwrini.ini";

  String sec_upp = section;
  String key_upp = key;
  sec_upp.toUpperCase();
  key_upp.toUpperCase();

  String s = "";
  String k = "";
  String v = "";
  bool section_found = false;
  bool key_found = false;

  String inifile = String(ini.name());
  int ind = inifile.lastIndexOf('/');
  String path = inifile.substring(0, ind+1);
  String tmpfile = path + tmpname;

  if (SPIFFS.exists(tmpfile)) SPIFFS.remove(tmpfile); // if exists tmpfile - remove it

  tmpini = SPIFFS.open(tmpfile, "w"); // open tmpfile
  if (tmpini) {
    ini.seek(0);
    tmpini.seek(0);
    while ( !ini_eof() ) { // find section
      s = ini_read_line();
      tmpini.println(s);  // save to new file
      // search for section
      if ( (s.length() < 3) || (s.charAt(0)=='#') || (s.charAt(0)==';') ) continue; // comment - get next line
      s.toUpperCase();
      if ( (s.charAt(0)=='[') && (s.charAt(s.length()-1)==']') ) { // it's section
        s.remove(0, 1);
        s.remove( (s.length()-1), 1);
        if ( s == sec_upp) { section_found = true; break; } // got it!
      }
    }
    // we are inside the section
    if (section_found) {
      key_found = false;
      while ( !ini_eof() ) { // find key
        s = ini_read_line();
        if ( (s.charAt(0)=='#') || (s.charAt(0)==';') ) { // comment - get next line
          tmpini.println(s);  // comment - save to new file
          continue;
        }
        s.toUpperCase();
        if ( (s.charAt(0)!='[') && (s.charAt(s.length()-1)!=']') ) {  // it is not next section...
          ind = s.indexOf('=');
          if (ind>=0) { // found "="
            k = s.substring(0, ind);
            v = s.substring(ind + 1);
            k.trim(); // trim key string
            v.trim(); // trim value string
            k.toUpperCase();
            if (k == key_upp) { // got key
              key_found = true;
              s = key + "=" + value;
              tmpini.println(s);  // new key - save to new file
              break;
            }
            else
              tmpini.println(s);  // not this key - save to new file
          }
          else
            tmpini.println(s);  // not a key - save to new file
        }
        else { // got next section - break
          tmpini.println(s);  // new section - save to new file
          break;
        }
      }
    }
    else { // there is no such section - save new one
       // save new section
      s = "[" + section + "]";
      tmpini.println(s);  // new section - save to new file
      // save new key
      s = key + "=" + value;
      tmpini.println(s);  // new section - save to new file
    }
    
    // save all to the end of file
    while ( !ini_eof() ) {
      s = ini_read_line();
      tmpini.println( s );
    }
    // end of write file

  }

  // close ini files
  if (tmpini) tmpini.close();
  if (ini   ) ini.close();
  SPIFFS.remove( inifile ); //remove old ini
  SPIFFS.rename( tmpfile, inifile ); // tmpini is now ini
  ini_open(inifile);

return "Rozmiar = "+ String(ini.size() );
}


bool ini_close() {
  if (ini) ini.close();
  return true;
}

/*
#include <string.h>
#include "FS.h"


void loop()
{

      f = SPIFFS.open(TESTFILE, "a");
      if (!f) {
        Serial.print("Unable To Open '");
      } else {
        Serial.print("Appending line to file '");
        Serial.print(TESTFILE);
        Serial.println("'");
        Serial.println();
        f.println("This line has been appended");
        f.close();
      }


*/
