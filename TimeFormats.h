#ifndef TimeFormats_h
#define TimeFormats_h

#include <string>

std::string formatTime12(unsigned long seconds);
std::string formatTime24(unsigned long seconds);
std::string formatTimeHM12(unsigned long seconds);
std::string formatTimeHM24(unsigned long seconds);
std::string formatTimeDecimalHours(unsigned long seconds);

#endif