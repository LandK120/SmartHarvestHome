#ifndef SystemLevel_h
#define SystemLevel_h

#include <string>
#include <Arduino.h>
#include "LCDMenu.h"
#include "Config.h"
#include "System.h"
#include "TimeFormats.h"

class System;
class LCDLine;
class LCDEditLine;
class LCDChoiceLine;
class LCDScreen;
class LCDSubScreen;

class SystemLevel{
public:
  SystemLevel(System* system, uint8_t level, uint8_t ledPin, uint8_t solPin);

  void setLED(bool state);
  void toggleLED();
  void setLEDTiming(long onPeriod, long offPeriod, long startDelay, bool startState);
  void setFDTiming(long cyclePeriod, long startDelay, bool startWithFD);
  uint8_t getLEDPin();
  uint8_t getSolPin();
  void setFDCycleTimings(long floodDuration, long drainDuration);
  void saveLevel();
  void loadLevel();

private:
  void initScheduleMenu(LCDScreen* scheduleScreen);
  void resetLEDLineVars();
  void resetFDLineVars();
  void resetAllLineVars();
  void OR_setLED(bool state);
  void OR_toggleLED();
  void OR_revertLED();
  void fullFDCycle();
  void OR_fullFDCycle();
  void OR_flood15();
  void OR_drain60();
  void OR_drain600();
  void OR_resetFD();
  void OR_resetLEDOn();
  void OR_resetLEDOff();
  void linkLevel1();
  void linkLevel2();
  void linkLevel3();
  void LEDonPeriodUpdate();
  void LEDoffPeriodUpdate();
  void updateLEDTiming(bool save = true);
  void updateFDTiming(bool save = true);
  void updateAllTiming();
  void checkEnabled();
  void tick(long secondsPassed);
  std::string formatTimeFuture(long seconds);

private:
  friend class System;

private:
  System* _system;
  uint8_t _level;
  char* _levelName;
  uint8_t _ledPin;
  uint8_t _solPin;
  bool _enabled;

  long _LEDonPeriodSeconds;
  long _LEDoffPeriodSeconds;
  bool _LEDstate;
  bool _ORLEDstate;
  long _LEDcounterSeconds;

  long _FDperiodSeconds;
  long _FDfloodSeconds;
  long _FDdrainSeconds;
  long _FDcounterSeconds;

private:
  LCDSubScreen* _timingScreen;

  LCDLine* _blankLine;
  LCDLine* _headerLine;
  LCDChoiceLine* _enableEdit;

  LCDScreen* _linkScreen;
  LCDLine* linkLine1;
  LCDLine* linkLine2;
  LCDLine* linkLine3;

  LCDEditLine* _LEDonPeriodEdit;
  LCDEditLine* _LEDoffPeriodEdit;
  LCDEditLine* _LEDstartDelayEdit;
  LCDChoiceLine* _LEDresetStateEdit;

  LCDEditLine* _FDperiodEdit;
  LCDEditLine* _FDstartDelayEdit;
  LCDChoiceLine* _FDresetStateEdit;

  LCDLine* _updateLEDLine;
  LCDLine* _updateFDLine ;
  LCDLine* _updateAllLine;
};

#endif