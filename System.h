#ifndef System_h
#define System_h

#include <string>
#include <Arduino.h>
#include "Rtc_Pcf8563.h" // Modified library (Joe Robertson)
#include "Config.h"
#include "LCDMenu.h"
#include "SystemLevel.h"
#include "TimeFormats.h"
#include "FlashAsEEPROM.h"
#include "Debug.h"
#include "Speaker.h"
#include "Sensors.h"

class LCDLine;
class LCDEditLine;
class LCDChoiceLine;
class LCDScreen;
class LCDSubScreen;
class SystemLevel;
    
class System{
public:
  System();

  void tick(long currentTime);
  void encoderClick();
  void encoderUp();
  void encoderDown();
  void encoderHold();

  uint8_t getPumpPin();
  bool unableToQueue(uint8_t numActions);
  void addActionToQueue(uint8_t pin, bool state, long holdTime);

  long getTime();
  void setClockMode();
  std::string formatTimeClock(long seconds);
  
  void resetState();
  void resetLEDORCounter();
  void setScreenSaver();
  void setStatusMessage(char* message);

  void linkLevels(uint8_t level, uint8_t copyFrom);
  void saveSystem();
  void loadSystem();

private:
  void initMenus();

  void OR_allLightsOn();
  void OR_allLightsOff();
  void OR_revertLights();
  void OR_floodAll30();
  void OR_drainAll60();
  void OR_drainAll600();
  void OR_enablePump60();
  void OR_disablePump();

  void LEDORTimeout();
  void resetClock();
  void setClock();
  bool wakeUp();

  void updateInfoScreen();
  void readSensors();

  void completeQueueAction();

  void DB_setHeartbeat();
  void DB_setHeartbeatVolume();
  
private:
  uint8_t _pumpPin;
  uint8_t _FDqueuePins[MAX_QUEUE_SIZE];
  bool _FDqueueStates[MAX_QUEUE_SIZE];
  long _FDqueueTimes[MAX_QUEUE_SIZE];
  int8_t _FDqueueIndex;
  long _counter;
  Rtc_Pcf8563 _rtc;
  long _rtcClockTime;
  long _lastRtcClockTime;
  long _clockTime;
  bool _clockMode;
  char _clockString[9];
  char _lastClockString[9];
  char _nextFDString[9];
  char _lastNextFDString[9];
  int8_t _temp;
  float _pH;
  long _eC;
  long _screenSaverTimeout;
  long _screenSaverCounter;
  long _LEDoverrideTimeout;
  long _LEDoverrideCounter;
  char _statusMessage[21];
  hd44780_I2Cexp* _lcd;
  bool _clockSetApply;
  long _rtcBackupTime;
  
  LCDMenu* _menu;
  SystemLevel* _levels[NUMBER_OF_LEVELS];

  Speaker _speaker;
  bool _heartbeat;

private:
  LCDScreen* _rootMenu;

  LCDScreen* _infoScreen;
  LCDLine* _infoTargetLine;
  LCDLine* _infoLine1;
  LCDLine* _infoLine2;
  LCDLine* _infoLine3;
  LCDLine* _infoLine4;

  LCDSubScreen* _schedulingScreen;

  LCDSubScreen* _overrideScreen;

  LCDLine* _blankLine;
  LCDLine* _headerLine;
  LCDLine* _floodAll;
  LCDLine* _drainAll60;
  LCDLine* _drainAll600;
  LCDLine* _enablePump;
  LCDLine* _disablePump;  
  LCDLine* _lightsOn;
  LCDLine* _lightsOff;
  LCDLine* _toggleLED1;
  LCDLine* _toggleLED2;
  LCDLine* _toggleLED3;
  LCDLine* _revertLEDs;
  LCDLine* _reset;
  LCDLine* _fullFD1;
  LCDLine* _fullFD2;
  LCDLine* _fullFD3;
  LCDLine* _flood1; 
  LCDLine* _flood2; 
  LCDLine* _flood3;
  LCDLine* _drain601;
  LCDLine* _drain602;
  LCDLine* _drain603;
  LCDLine* _drain6001;
  LCDLine* _drain6002;
  LCDLine* _drain6003;
  LCDLine* _restartFD1;
  LCDLine* _restartFD2;
  LCDLine* _restartFD3;
  LCDLine* _restartLED1On;
  LCDLine* _restartLED2On;
  LCDLine* _restartLED3On;
  LCDLine* _restartLED1Off;
  LCDLine* _restartLED2Off;
  LCDLine* _restartLED3Off;

  LCDSubScreen* _configScreen;
  LCDChoiceLine* _clockModeEdit;
  LCDEditLine* _clockSetEdit;
  LCDLine* _clockSetApplyLine;

  LCDScreen* _aboutScreen;
  LCDLine* _aboutTargetLine;
  LCDLine* _aboutLine01;
  LCDLine* _aboutLine02;
  LCDLine* _aboutLine03;
  LCDLine* _aboutLine04;
  LCDLine* _aboutLine05;
  LCDLine* _aboutLine06;
  LCDLine* _aboutLine07;
  LCDLine* _aboutLine08;
  LCDLine* _aboutLine09;
  LCDLine* _aboutLine10;
  LCDLine* _aboutLine11;
  LCDLine* _aboutLine12;
  LCDLine* _aboutLine13;
  LCDLine* _aboutLine14;
  LCDLine* _aboutLine15;
  LCDLine* _aboutLine16;
  LCDLine* _aboutLine17;
  LCDLine* _aboutLine18;

  LCDScreen* _screenSaver;

  LCDSubScreen* _debugScreen;
  LCDChoiceLine* _debugHeartbeat;
  LCDEditLine* _debugHeartbeatVolume;
};

#endif