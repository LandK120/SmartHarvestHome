#include "TimeFormats.h"

std::string formatTime12(unsigned long seconds){
  unsigned long hours = seconds / 3600;
  unsigned long minutes = (seconds % 3600) / 60;
  seconds = (seconds % 3600) % 60;
  std::string suffix = (hours >= 12) ? " PM" : " AM";
  hours = ((hours + 11) % 12) + 1;
  std::string str_hours = (hours < 10) ? "0" + std::to_string(hours) : std::to_string(hours);
  std::string str_minutes = (minutes < 10) ? "0" + std::to_string(minutes) : std::to_string(minutes);
  std::string str_seconds = std::to_string(seconds);
  return str_hours + ":" + str_minutes + ":" + str_seconds;
}

std::string formatTime24(unsigned long seconds){
  unsigned long hours = seconds / 3600;
  unsigned long minutes = (seconds % 3600) / 60;
  seconds = (seconds % 3600) % 60;
  std::string str_hours = (hours < 10) ? "0" + std::to_string(hours) : std::to_string(hours);
  std::string str_minutes = (minutes < 10) ? "0" + std::to_string(minutes) : std::to_string(minutes);
  std::string str_seconds = std::to_string(seconds);
  return str_hours + ":" + str_minutes + ":" + str_seconds;
}

std::string formatTimeHM12(unsigned long seconds){
  unsigned long hours = seconds / 3600;
  unsigned long minutes = (seconds % 3600) / 60;
  std::string suffix = (hours >= 12) ? " PM" : " AM";
  hours = ((hours + 11) % 12) + 1;
  std::string str_hours = (hours < 10) ? "0" + std::to_string(hours) : std::to_string(hours);
  std::string str_minutes = (minutes < 10) ? "0" + std::to_string(minutes) : std::to_string(minutes);
  return str_hours + ":" + str_minutes + suffix;
}

std::string formatTimeHM24(unsigned long seconds){
  unsigned long hours = seconds / 3600;
  unsigned long minutes = (seconds % 3600) / 60;
  std::string str_hours = (hours < 10) ? "0" + std::to_string(hours) : std::to_string(hours);
  std::string str_minutes = (minutes < 10) ? "0" + std::to_string(minutes) : std::to_string(minutes);
  return str_hours + ":" + str_minutes;
}

std::string formatTimeDecimalHours(unsigned long seconds){
  unsigned long hours = seconds / 3600;
  unsigned long decimal = (seconds % 3600) / 360;
  return std::to_string(hours) + "." + std::to_string(decimal) + "hr";
}