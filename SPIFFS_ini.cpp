/*
    ----------------------   SPIFFS_ini ver. 2.0  ----------------------
      (c) 2020 SpeedBit, reg. Czestochowa, Poland
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


#include <SPIFFS_ini.h>
#include "LittleFS.h"
#include "stdlib.h"

File ini;
String ini_fname = "";
char *p_ini_file = NULL;
unsigned long pos_ini_file  = 0;
unsigned long size_ini_file = 0;
char *p_tmp_file = NULL;
unsigned long pos_tmp_file  = 0;
unsigned long size_tmp_file = 0;

#define LINE_SIZE 256
#define INI_EOF   0x00


bool ini_open (String ini_name) {
  unsigned long cnt = 0;

  if (ini) ini_close();

  if (LittleFS.exists(ini_name)) ini = LittleFS.open(ini_name, "r+");
  if (ini.size() > 0) {
    ini_fname = ini_name;
    size_ini_file = ini.size();
    p_ini_file = (char *) malloc( size_ini_file );
    if (p_ini_file != NULL) {
      cnt = ini.readBytes(p_ini_file, size_ini_file );
      if (cnt == size_ini_file ) {
        pos_ini_file = 0;
        pos_tmp_file = 0;
        p_tmp_file   = NULL;
        return ini;
      }
    }
  }
  ini_close();
  return false;
}



bool ini_eof() {
  if (!ini) return true;
  return ( pos_ini_file >= size_ini_file );
}



char peek_char() {
  if ( ini_eof() ) return INI_EOF;
  return *(p_ini_file + pos_ini_file);
}



char get_char() {
  char c;
  if ( ini_eof() ) return INI_EOF;
  c = peek_char();
  if ( c == INI_EOF ) return INI_EOF;
  pos_ini_file++;
  return c;
}



String ini_read_line() {
  if (!ini) return "";

  String  s = "";
  char    c = 0;
  bool test = false;

  do { // read string until CR or LF
    c = get_char();
    test = (c != INI_EOF) && (c != 13) && (c != 10); // not CR and not LF
    if (test) s +=  c;
  } while (test);

  // if last char was CR then read LF (if exists)
  if ( (c==13)  && !ini_eof() ) {
    c = peek_char();
    if (c == 10) pos_ini_file++;
  }

  s.trim(); // trim string
  return s;
}


String ini_read(String section, String key, String def) {
  if (!ini) return "";

  section.toUpperCase();
  key.toUpperCase();

  String s = "";
  String k = "";
  String v = "";
  bool found = false;
  int  ind   = 0;

  section.trim();
  key.trim();
  def.trim();

  pos_ini_file = 0;
  while (!ini_eof() ) { // find section
    s = ini_read_line();
    if ( (s == "") || (s.charAt(0) == '#') || (s.charAt(0) == ';') ) continue; // comment - get next line
    s.toUpperCase();
    if ( (s.charAt(0) == '[') && (s.charAt(s.length() - 1) == ']') ) { // it's section
      if ( s == "[" + section + "]") { found = true; break; } // got it!
    }
  }

  if (found) { // got section
    found = false;
    while (!ini_eof() ) { // find key
      s = ini_read_line();
      if ( (s == "") || (s.charAt(0) == '#') || (s.charAt(0) == ';') ) continue; // comment - get next line
      if ( (s.charAt(0) != '[') && (s.charAt(s.length() - 1) != ']') ) {  // it is not next section...
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



bool ini_put_char(char c) {
  if (!p_tmp_file) return false;
  if (pos_tmp_file < size_tmp_file) {
    *(p_tmp_file + pos_tmp_file) = c;
    pos_tmp_file++;
  }
  return true;
}


bool ini_write_line(String s) {
  if (!p_tmp_file) return false;
  if (s.length() > 0) {
    for (int i = 0; i < s.length(); i++) { ini_put_char(s[i]); }
  }
  ini_put_char('\r');
  ini_put_char('\n');
  return true;
}


bool ini_write(String section, String key, String value) {
  if (!ini) return false;
  bool res = false;

  section.trim();
  key.trim();

  String sec_upp = section;
  String key_upp = key;
  sec_upp.toUpperCase();
  key_upp.toUpperCase();

  String s = "";
  String k = "";
  String v = "";
	String line = "";
  bool section_found = false;
  bool key_found = false;
  int  ind       = 0;
  int max_line_length = 2 + section.length() + 2 + (key.length() + value.length() + 3) + 2 + 2; // "<cr><lf>[section]<cr><lf>key = value<cr><lf>"

  if ( max_line_length > LINE_SIZE) return false;
  if (p_tmp_file) free(p_tmp_file); // if exists p_tmp_file - free it

  size_tmp_file = size_ini_file + max_line_length + 2; // +2 -> CR+LF
  p_tmp_file = (char *) malloc( size_tmp_file ); // open p_tmp_file

  if (p_tmp_file) {
    pos_ini_file = 0;
    pos_tmp_file = 0;

    while ( !ini_eof() ) { // find section
      s = ini_read_line();
      // save to new file
      ini_write_line(s);
      // search for section
      if ( (s.length() != (sec_upp.length() + 2) ) || (s == "") || (s.charAt(0) == '#') || (s.charAt(0) == ';') ) continue; // comment - get next line
      s.toUpperCase();
      if ( (s.charAt(0) == '[') && (s.charAt(s.length() - 1) == ']') ) { // it's section
        if ( s == "[" + sec_upp + "]" ) { section_found = true; break; } // got it!
      }
    }

    // we are inside the section
    if (section_found) {
      key_found = false;
      while ( !ini_eof() ) { // find key
        line = ini_read_line();
			  s = line;
        if ( (s.charAt(0) == '#') || (s.charAt(0) == ';') ) { // comment - get next line
          // comment - save to new file
          ini_write_line( s );
          continue;
        }
        //if ( s.length() == 0 ) { continue; } // empty line - not save to new file
        s.toUpperCase();
        if ( (s.charAt(0) != '[') && (s.charAt(s.length() - 1) != ']') ) {  // it is not next section...
          ind = s.indexOf('=');
          if (ind>=0) { // found "="
            k = s.substring(0, ind);
            v = s.substring(ind + 1);
            k.trim(); // trim key string
            v.trim(); // trim value string
            k.toUpperCase();
            if (k == key_upp) { // got key
              key_found = true;
              res = true;
              // new key - save to new file
              ini_write_line(  key + " = " + value );
              break;
            }
            else {
              // not this key - save to new file
              ini_write_line( line );
            }
          }
          else {
            // not a key - save to new file
            ini_write_line( line );
          }
        } // line - not section
        else { // got next section - break
          // new key - save to new file
          ini_write_line(  key + " = " + value );
          // new section - save to new file
          ini_write_line( line );
          break;
        }
      } // find key
      if (ini_eof() && section_found && !key_found) {
        ini_write_line(  key + " = " + value );
      }
    } // section found
    else { // there is no such section - save new one
      // one new line before section
      //ini_write_line( "" );
      // save new section
      ini_write_line( "[" + section + "]" );
      // save new key
      ini_write_line( key + " = " + value );
      res = true;
    }

    // save all to the end of file
    while ( !ini_eof() ) {
      s = ini_read_line();
      ini_write_line( s );
    }
  }

  free(p_ini_file);         //remove old ini
  p_ini_file =  (char *) malloc(pos_tmp_file);
  memcpy(p_ini_file, p_tmp_file, pos_tmp_file);
  size_ini_file = pos_tmp_file;
  pos_ini_file  = 0;

  free(p_tmp_file);         //remove old tmp
  p_tmp_file    = NULL;
  pos_tmp_file  = 0;

  return res;
}



bool ini_delete_key(String section, String key) {
  if (!ini) return false;
  bool res = false;

  section.trim();
  key.trim();

  String sec_upp = section;
  String key_upp = key;
  sec_upp.toUpperCase();
  key_upp.toUpperCase();

  String s = "";
  String k = "";
  String v = "";
	String line = "";
  bool section_found = false;
  bool key_found = false;
  int  ind       = 0;

  if (p_tmp_file) free(p_tmp_file); // if exists p_tmp_file - free it

  p_tmp_file = (char *) malloc( size_ini_file );
  if (p_tmp_file) {
    pos_ini_file = 0;
    pos_tmp_file = 0;

    while ( !ini_eof() ) { // find section
      s = ini_read_line();
      // save to new file
      ini_write_line(s);
      // search for section
      if ( (s.length() != (sec_upp.length() + 2) ) || (s == "") || (s.charAt(0) == '#') || (s.charAt(0) == ';') ) continue; // comment - get next line
      s.toUpperCase();
      if ( (s.charAt(0) == '[') && (s.charAt(s.length() - 1) == ']') ) { // it's section
        if ( s == "[" + sec_upp + "]" ) { section_found = true; break; } // got it!
      }
    }

    // we are inside the section
    if (section_found) {
      key_found = false;
      while ( !ini_eof() ) { // find key
        line = ini_read_line();
			  s = line;
        if ( (s == "") || (s.charAt(0) == '#') || (s.charAt(0) == ';') ) { // comment - get next line
          // comment - save to new file
          ini_write_line(s);
          continue;
        }
        s.toUpperCase();
        if ( ( (s.charAt(0) != '[') && (s.charAt(s.length() - 1) != ']') ) || (!ini_eof()) ) {  // it is not next section...
          ind = s.indexOf('=');
          if (ind>=0) { // found "="
            k = s.substring(0, ind);
            v = s.substring(ind + 1);
            k.trim(); // trim key string
            v.trim(); // trim value string
            k.toUpperCase();
            if (k == key_upp) { // got key
              key_found = true;
              res = true;
              // key is not save to new file -> key is deleted
              break;
            }
            else {
              // not this key - save to new file
              ini_write_line( line );
            }
          }
          else {
            // not a key - save to new file
            ini_write_line( line );
          }
        }
        else { // got next section - break
          // new section - save to new file
          ini_write_line( line );
          break;
        }
      } // find key
    }
    else { // there is no such section
      res = false;
    }

    // save all to the end of file
    while ( !ini_eof() ) {
      s = ini_read_line();
      ini_write_line( s );
    }
  }

  free(p_ini_file);         //remove old ini
  p_ini_file =  (char *) malloc(pos_tmp_file);
  memcpy(p_ini_file, p_tmp_file, pos_tmp_file);
  size_ini_file = pos_tmp_file;
  pos_ini_file  = 0;

  free(p_tmp_file);         //remove old tmp
  p_tmp_file    = NULL;
  pos_tmp_file  = 0;

  return res;
}





bool ini_delete_section(String section) {
  if (!ini) return false;
  bool res = false;

  section.trim();

  String sec_upp = section;
  sec_upp.toUpperCase();

  String s = "";
	String line = "";
  bool section_found = false;

  if (p_tmp_file) free(p_tmp_file); // if exists p_tmp_file - free it

  p_tmp_file = (char *) malloc( size_ini_file );

  if (p_tmp_file) {
    pos_ini_file = 0;
    pos_tmp_file = 0;

    while ( !ini_eof() ) { // find section
      line = ini_read_line();
      s = line;
      s.toUpperCase();
      if ( (s.charAt(0) == '[') && (s.charAt(s.length() - 1) == ']') ) { // it's section
        if ( s == "[" + sec_upp + "]" ) { section_found = true; break; } // got it!
      }
      // save to new file
      ini_write_line(line);
    }

    // we are inside the section
    if (section_found) {
      while ( !ini_eof() ) { // find next section
        line = ini_read_line();
        s = line;
        s.toUpperCase();
        if ( (s.charAt(0) == '[') && (s.charAt(s.length() - 1) == ']') ) { // it's next section
          // save to new file
          ini_write_line(line);
          break;
        }
        else {
          // do nothing - all keys are deleted
        }
      }
    }
    else { // there is no such section
      res = false;
    }

    // save all to the end of file
    while ( !ini_eof() ) {
      s = ini_read_line();
      ini_write_line( s );
    }
  }

  free(p_ini_file);         //remove old ini
  p_ini_file =  (char *) malloc(pos_tmp_file);
  memcpy(p_ini_file, p_tmp_file, pos_tmp_file);
  size_ini_file = pos_tmp_file;
  pos_ini_file  = 0;

  free(p_tmp_file);         //remove old tmp
  p_tmp_file    = NULL;
  pos_tmp_file  = 0;

  return res;
}




bool ini_close() {
  if (ini) ini.close();
  ini = LittleFS.open(ini_fname, "w");
  ini.write(p_ini_file, size_ini_file);
  if (ini) ini.close();

  if (p_ini_file) free(p_ini_file);
  pos_ini_file  = 0;
  size_ini_file = 0;
  if (p_tmp_file) free(p_tmp_file);
  pos_tmp_file  = 0;

  ini_fname = "";
  return true;
}
