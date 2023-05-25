#ifndef Debug_h
#define Debug_h

#include <Arduino.h>
#include "Config.h"
void debug(char* string);
void debug(long l);
void debug(int i);
void debug(bool b);
void debug(byte b);

#endif