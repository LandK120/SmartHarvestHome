#include <string>
#include "Debug.h"

void debug(char* string){
  if(DEBUG){
    long now = millis();
    Serial.print(now);
    Serial.print("\t");
    Serial.println(string);
  }
}

void debug(long l){
  debug((char*)(std::to_string(l).c_str()));
}

void debug(int i){
  debug((char*)(std::to_string(i).c_str()));
}

void debug(bool b){
  debug((char*)(std::to_string(b).c_str()));
}

void debug(byte b){
  debug((char*)(std::to_string(b).c_str()));
}