#ifndef Sensors_h
#define Sensors_h

#include <Arduino.h> // 1,2,3,7,8
#include "Config.h"

// ----- Thermistor Data -----
// NTCLE100E3682*B0
// R25 - 6800
#define THERM_A 0.003354016
#define THERM_B 0.000256985
#define THERM_C 0.000002620131
#define THERM_D 0.00000006383091
#define THERM_R 9910.0

float readTemp();
float read_pH();
float readEC();

#endif