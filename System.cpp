#include "System.h"

System::System(){

  _temp = 0;
  _eC = 0;
  _pH = 0;

  if (DEBUG){
    Serial.begin(9600);
    debug("init");
    delay(4000);
    debug("delay complete");
  }

  _pumpPin = PIN_PUMP;

  for (uint8_t i = 0; i < MAX_QUEUE_SIZE; i++){
    _FDqueuePins[i] = 0;
    _FDqueueStates[i] = false;
    _FDqueueTimes[i] = 0;
  }

  if(NO_RTC){
    _clockTime = 0;
    _rtcClockTime = 0;
    _lastRtcClockTime = 0;
  } else {
    _rtc.initClock();
    debug("rtc initiated");
    _rtcClockTime = _rtc.getClockSeconds();
    _lastRtcClockTime = _rtcClockTime;
    _clockTime = _rtcClockTime;
  }

  _FDqueueIndex = -1;
  _counter = 0;
  _clockMode = DEFAULT_CLOCK_MODE;
  _screenSaverTimeout = SCREENSAVER_TIMEOUT;
  _screenSaverCounter = _screenSaverTimeout;
  _LEDoverrideTimeout = LED_OVERRIDE_TIMEOUT;
  _LEDoverrideCounter = _LEDoverrideTimeout;
  setStatusMessage("All systems normal");

  readSensors();
  debug("Sensors read");

  _lcd = new hd44780_I2Cexp();
  _lcd->begin(LCD_COLS, LCD_ROWS);
  _lcd->clear();
  _lcd->setCursor(0, 0);
  _lcd->print("Error Initializing");
  _lcd->setCursor(0,1);
  _lcd->print("Reset System");
  debug("lcd initialized");

  _menu = new LCDMenu(LCD_COLS, LCD_ROWS, _lcd);

  _levels[0] = new SystemLevel(this, 0, PIN_LED_1, PIN_SOL_1);
  _levels[1] = new SystemLevel(this, 1, PIN_LED_2, PIN_SOL_2);
  _levels[2] = new SystemLevel(this, 2, PIN_LED_3, PIN_SOL_3);
  _levels[0]->setFDCycleTimings(FD_CYCLE_FLOOD_SECONDS_1, FD_CYCLE_DRAIN_SECONDS_1);
  _levels[1]->setFDCycleTimings(FD_CYCLE_FLOOD_SECONDS_2, FD_CYCLE_DRAIN_SECONDS_2);
  _levels[2]->setFDCycleTimings(FD_CYCLE_FLOOD_SECONDS_3, FD_CYCLE_DRAIN_SECONDS_3);
  debug("levels initiated");

  initMenus();
  debug("menus loaded");
  updateInfoScreen();
  debug("info screen refreshed");
  _menu->update();

  // load saved variables from flash if saved
  if (EEPROM.isValid()){
    debug("initiating eeprom");
    loadSystem();
    debug("eeprom loaded");
  } else {
    debug("eeprom loading skipped");
  }

  _speaker.startUpTones();
  _heartbeat = DEBUG_MENU;

  _rtcBackupTime = millis();
}


void System::initMenus(){
  _rootMenu = new LCDScreen();

  // -------- Info screen --------
  _infoScreen = new LCDScreen();
  _infoScreen->hideCursor();
  _infoTargetLine = new LCDLine(_rootMenu, "Info");
  _infoTargetLine->setTarget(_infoScreen);

  _infoLine1 = new LCDLine(_infoScreen, "Temp ", _temp, "*C   ", _clockString); //°
  _infoLine2 = new LCDLine(_infoScreen, "EC ", _eC, " ppm   pH ", _pH);
  _infoLine3 = new LCDLine(_infoScreen, "Next FD Cycle: ", _nextFDString);
  _infoLine4 = new LCDLine(_infoScreen, "", _statusMessage);
  
  _infoLine1->setTarget(_rootMenu);
  _infoLine2->setTarget(_rootMenu);
  _infoLine3->setTarget(_rootMenu);
  _infoLine4->setTarget(_rootMenu);
  _infoLine1->setDecimalPlaces(0);

  // -------- Scheduling --------
  _schedulingScreen = new LCDSubScreen(_rootMenu, "Scheduling");
  _levels[0]->initScheduleMenu(_schedulingScreen);
  _levels[1]->initScheduleMenu(_schedulingScreen);
  _levels[2]->initScheduleMenu(_schedulingScreen);
  
  // -------- Override --------
  _overrideScreen = new LCDSubScreen(_rootMenu, "Override");

  _blankLine = new LCDLine(_overrideScreen);
  _headerLine = new LCDLine(_overrideScreen, "--- Full System ---");
  _floodAll = new LCDLine(_overrideScreen, "Flood all (30s)");
  _drainAll60 = new LCDLine(_overrideScreen, "Drain all (60s)");
  _drainAll600 = new LCDLine(_overrideScreen, "Drain all (10m)");
  _floodAll->setClickFunction([this](){OR_floodAll30();});
  _drainAll60->setClickFunction([this](){OR_drainAll60();});
  _drainAll600->setClickFunction([this](){OR_drainAll600();});

  _blankLine = new LCDLine(_overrideScreen);
  _headerLine = new LCDLine(_overrideScreen, "- Clean Reservoir -");
  _enablePump = new LCDLine(_overrideScreen, "Enable pump (60s)");
  _disablePump = new LCDLine(_overrideScreen, "Disable pump");
  _enablePump->setClickFunction([this](){OR_enablePump60();});
  _disablePump->setClickFunction([this](){OR_disablePump();});

  _blankLine = new LCDLine(_overrideScreen);
  _headerLine = new LCDLine(_overrideScreen, "--- Adjust LEDs ---");
  _lightsOn = new LCDLine(_overrideScreen, "All LEDs on");
  _lightsOff = new LCDLine(_overrideScreen, "All LEDs off");
  _toggleLED1 = new LCDLine(_overrideScreen, "Toggle LED level 1");
  _toggleLED2 = new LCDLine(_overrideScreen, "Toggle LED level 2");
  _toggleLED3 = new LCDLine(_overrideScreen, "Toggle LED level 3");
  _revertLEDs = new LCDLine(_overrideScreen, "Revert LEDs");
  _lightsOn->setClickFunction([this](){OR_allLightsOn();});
  _lightsOff->setClickFunction([this](){OR_allLightsOff();});
  _toggleLED1->setClickFunction([this](){_levels[0]->OR_toggleLED();});
  _toggleLED2->setClickFunction([this](){_levels[1]->OR_toggleLED();});
  _toggleLED3->setClickFunction([this](){_levels[2]->OR_toggleLED();});
  _revertLEDs->setClickFunction([this](){OR_revertLights();});

  _blankLine = new LCDLine(_overrideScreen);
  _headerLine = new LCDLine(_overrideScreen, "- Override Queue -");
  _reset = new LCDLine(_overrideScreen, "Clear queue");
  _fullFD1 = new LCDLine(_overrideScreen, "Full F/D cycle 1");
  _fullFD2 = new LCDLine(_overrideScreen, "Full F/D cycle 2");
  _fullFD3 = new LCDLine(_overrideScreen, "Full F/D cycle 3");
  _flood1 = new LCDLine(_overrideScreen, "Flood level 1 (15s)");
  _flood2 = new LCDLine(_overrideScreen, "Flood level 2 (15s)");
  _flood3 = new LCDLine(_overrideScreen, "Flood level 3 (15s)");
  _drain601 = new LCDLine(_overrideScreen, "Drain level 1 (60s)");
  _drain602 = new LCDLine(_overrideScreen, "Drain level 2 (60s)");
  _drain603 = new LCDLine(_overrideScreen, "Drain level 3 (60s)");
  _drain6001 = new LCDLine(_overrideScreen, "Drain level 1 (10m)");
  _drain6002 = new LCDLine(_overrideScreen, "Drain level 2 (10m)");
  _drain6003 = new LCDLine(_overrideScreen, "Drain level 3 (10m)");
  _reset->setClickFunction([this](){resetState();});
  _fullFD1->setClickFunction([this](){_levels[0]->OR_fullFDCycle();});
  _fullFD2->setClickFunction([this](){_levels[1]->OR_fullFDCycle();});
  _fullFD3->setClickFunction([this](){_levels[2]->OR_fullFDCycle();});
  _flood1->setClickFunction([this](){_levels[0]->OR_flood15();});
  _flood2->setClickFunction([this](){_levels[1]->OR_flood15();});
  _flood3->setClickFunction([this](){_levels[2]->OR_flood15();});
  _drain601->setClickFunction([this](){_levels[0]->OR_drain60();});
  _drain602->setClickFunction([this](){_levels[1]->OR_drain60();});
  _drain603->setClickFunction([this](){_levels[2]->OR_drain60();});
  _drain6001->setClickFunction([this](){_levels[0]->OR_drain600();});
  _drain6002->setClickFunction([this](){_levels[1]->OR_drain600();});
  _drain6003->setClickFunction([this](){_levels[2]->OR_drain600();});

  _blankLine = new LCDLine(_overrideScreen);
  _headerLine = new LCDLine(_overrideScreen, "--- Restart Now ---");
  _restartFD1 = new LCDLine(_overrideScreen, "Restart F/D cycle 1");
  _restartFD2 = new LCDLine(_overrideScreen, "Restart F/D cycle 2");
  _restartFD3 = new LCDLine(_overrideScreen, "Restart F/D cycle 3");
  _restartLED1On = new LCDLine(_overrideScreen, "Restart LED 1 (on)");
  _restartLED2On = new LCDLine(_overrideScreen, "Restart LED 2 (on)");
  _restartLED3On = new LCDLine(_overrideScreen, "Restart LED 3 (on)");
  _restartLED1Off = new LCDLine(_overrideScreen, "Restart LED 1 (off)");
  _restartLED2Off = new LCDLine(_overrideScreen, "Restart LED 2 (off)");
  _restartLED3Off = new LCDLine(_overrideScreen, "Restart LED 3 (off)");
  _restartFD1->setClickFunction([this](){_levels[0]->OR_resetFD();});
  _restartFD2->setClickFunction([this](){_levels[1]->OR_resetFD();});
  _restartFD3->setClickFunction([this](){_levels[2]->OR_resetFD();});
  _restartLED1On->setClickFunction([this](){_levels[0]->OR_resetLEDOn();});
  _restartLED2On->setClickFunction([this](){_levels[1]->OR_resetLEDOn();});
  _restartLED3On->setClickFunction([this](){_levels[2]->OR_resetLEDOn();});   
  _restartLED1Off->setClickFunction([this](){_levels[0]->OR_resetLEDOff();});
  _restartLED2Off->setClickFunction([this](){_levels[1]->OR_resetLEDOff();});
  _restartLED3Off->setClickFunction([this](){_levels[2]->OR_resetLEDOff();});

  // -------- Configuration --------
  _configScreen = new LCDSubScreen(_rootMenu, "Configuration");
  _configScreen->setOnEntryClickFunction([this](){resetClock();});
  const char* choices[] = {"AM/PM", "24 hr"};
  _clockModeEdit = new LCDChoiceLine(_configScreen, 2, (char**)choices, "Clock mode: ", [this](){setClockMode();});
  _clockSetEdit = new LCDEditLine(_configScreen, "Set clock: ", _clockTime, 0, SECONDS_IN_ONE_DAY, 3600, [this](long seconds) -> std::string{return this->formatTimeClock(seconds);}, [this](){setClock();});

  // -------- About --------
  _aboutScreen = new LCDScreen();
  _aboutScreen->hideCursor();
  _aboutTargetLine = new LCDLine(_rootMenu, "About");
  _aboutTargetLine->setTarget(_aboutScreen);

  _aboutLine01 = new LCDLine(_aboutScreen, " Smart Harvest Home ");
  _aboutLine02 = new LCDLine(_aboutScreen, "         -          ");
  _aboutLine03 = new LCDLine(_aboutScreen, "   BCIT 4th Year    ");
  _aboutLine04 = new LCDLine(_aboutScreen, " Mech. Engineering  ");
  _aboutLine05 = new LCDLine(_aboutScreen, "  Capstone Project  ");
  _aboutLine06 = new LCDLine(_aboutScreen, "         -          ");
  _aboutLine07 = new LCDLine(_aboutScreen, "  Designed & Built  ");
  _aboutLine08 = new LCDLine(_aboutScreen, "  in 2022/2023 by:  ");
  _aboutLine09 = new LCDLine(_aboutScreen, "   Connor Eisler    ");
  _aboutLine10 = new LCDLine(_aboutScreen, "      Ethan Ha      ");
  _aboutLine11 = new LCDLine(_aboutScreen, "  Michal Jaworski   ");
  _aboutLine12 = new LCDLine(_aboutScreen, "   Lewis Karpiuk    ");
  _aboutLine13 = new LCDLine(_aboutScreen, "         -          ");
  _aboutLine14 = new LCDLine(_aboutScreen, " Special Thanks to: ");
  _aboutLine15 = new LCDLine(_aboutScreen, "    Vahid Askari    ");
  _aboutLine16 = new LCDLine(_aboutScreen, "    Johan Fourie    ");
  _aboutLine17 = new LCDLine(_aboutScreen, "   Chris Townsend   ");
  _aboutLine18 = new LCDLine(_aboutScreen, "    Jason Brett     ");
  _aboutLine01->setTarget(_rootMenu);
  _aboutLine02->setTarget(_rootMenu);
  _aboutLine03->setTarget(_rootMenu);
  _aboutLine04->setTarget(_rootMenu);
  _aboutLine05->setTarget(_rootMenu);
  _aboutLine06->setTarget(_rootMenu);
  _aboutLine07->setTarget(_rootMenu);
  _aboutLine08->setTarget(_rootMenu);
  _aboutLine09->setTarget(_rootMenu);
  _aboutLine10->setTarget(_rootMenu);
  _aboutLine11->setTarget(_rootMenu);
  _aboutLine12->setTarget(_rootMenu);
  _aboutLine13->setTarget(_rootMenu);
  _aboutLine14->setTarget(_rootMenu);
  _aboutLine15->setTarget(_rootMenu);
  _aboutLine16->setTarget(_rootMenu);
  _aboutLine17->setTarget(_rootMenu);
  _aboutLine18->setTarget(_rootMenu);

  // -------- Debug --------
  if(DEBUG_MENU){
    _debugScreen = new LCDSubScreen(_rootMenu, "DEBUG");
    const char* choices[] = {"No", "Yes"};
    _debugHeartbeat = new LCDChoiceLine(_debugScreen, 2, (char**)choices, "Heartbeat: ", [this](){DB_setHeartbeat();}, 1);
    _debugHeartbeatVolume = new LCDEditLine(_debugScreen, "Heartbeat Vol: ", 255, 0, 255, 5, nullptr, [this](){DB_setHeartbeatVolume();});
  }

  // -------- Screen saver --------
  _screenSaver = new LCDScreen();
  _screenSaver->hideCursor();
  _blankLine = new LCDLine(_screenSaver, "");
  _blankLine->setTarget(_infoScreen);

  // -------- Finalize menu --------
  _menu->setRoot(_infoScreen);
  _menu->setScreen(_infoScreen, false);
}

void System::tick(long currentTime){
  long secondsPassed;

  if(NO_RTC){ // debug
    secondsPassed = 1;
    _clockTime++;
    if (_clockTime >= SECONDS_IN_ONE_DAY) _clockTime = 0;
  } 
  else {
    _lastRtcClockTime = _rtcClockTime;
    _rtcClockTime = _rtc.getClockSeconds();
    if (_rtcClockTime == _lastRtcClockTime){
      if (currentTime - _rtcBackupTime < MAX_RTC_DISCONNECT) return;
      
      _rtc.initClock(); // attempt to reset
      secondsPassed = (currentTime - _rtcBackupTime) / 1000;
      _clockTime += secondsPassed;
      setStatusMessage("Error: No RTC Comm");

    } else {
      if (abs(_lastRtcClockTime - _rtcClockTime) > 5){
        debug("ERROR: time difference > 5");
        return;
      }

      // determine seconds passed
      if (_rtcClockTime > _clockTime) secondsPassed = _rtcClockTime - _clockTime;
      else secondsPassed = _rtcClockTime + SECONDS_IN_ONE_DAY - _clockTime;
      _clockTime = _rtcClockTime;
    }
  }

  _rtcBackupTime = currentTime;

  //debug("tick");
  //if (_heartbeat) _speaker.beep();

  // level tick (potentially updating queue)
  for (uint8_t i = 0; i < NUMBER_OF_LEVELS; i++){
    _levels[i]->tick(secondsPassed);
  }

  // change counters
  if (_counter > 0) _counter -= secondsPassed;
  if (_screenSaverCounter > 0) _screenSaverCounter -= secondsPassed;
  if (_LEDoverrideCounter > 0) _LEDoverrideCounter -= secondsPassed;

  // take appropriate actions
  if (_counter <= 0 && _FDqueueIndex >= 0) completeQueueAction();
  if (_screenSaverCounter <= 0 && _screenSaverTimeout > 0) setScreenSaver();
  if (_LEDoverrideCounter <= 0) LEDORTimeout();

  updateInfoScreen();
}

bool System::wakeUp(){
  // if waking from sleep
  debug("calling wakeUp()");
  if (_screenSaverCounter <= 0 && _screenSaverTimeout > 0){
    debug("woken up");
    _lcd->begin(LCD_COLS, LCD_ROWS);
    updateInfoScreen();
    _menu->setScreen(_infoScreen);
    _lcd->backlight();
    _screenSaverCounter = _screenSaverTimeout;
    return true;
  }
  _screenSaverCounter = _screenSaverTimeout;
  return false;
}

void System::encoderClick(){
  if (!wakeUp()) _menu->click();
}

void System::encoderUp(){
  if (!wakeUp()) _menu->up();
}

void System::encoderDown(){
  if (!wakeUp()) _menu->down();
}

void System::encoderHold(){
  if (!wakeUp()){
    updateInfoScreen();
    _menu->hold();
  }
}

uint8_t System::getPumpPin(){
  return _pumpPin;
}

bool System::unableToQueue(uint8_t numActions){
  return _FDqueueIndex >= MAX_QUEUE_SIZE - numActions;
}

void System::addActionToQueue(uint8_t pin, bool state, long holdTime){
  if (_FDqueueIndex >= MAX_QUEUE_SIZE - 1) return;
  _FDqueueIndex += 1;
  _FDqueuePins[_FDqueueIndex] = pin;
  _FDqueueStates[_FDqueueIndex] = state;
  _FDqueueTimes[_FDqueueIndex] = holdTime;
}

long System::getTime(){
  return _clockTime;
}

void System::setClockMode(){
  _clockMode = _clockModeEdit->getValue();
  saveSystem();
}

void System::resetClock(){
  _clockModeEdit->setValue(int(_clockMode));
  _clockSetEdit->setValue(_clockTime);
  _clockSetEdit->setStep(3600);
  _clockSetApply = false;
}

void System::setClock(){
  if (_clockSetApply){
    _clockSetEdit->setStep(3600);
    _clockTime = _clockSetEdit->getValue();
    long hours = _clockTime / 3600;
    long minutes = (_clockTime % 3600) / 60;
    long seconds = (_clockTime % 3600) % 60;
    _rtc.setTime(hours, minutes, seconds);
    updateInfoScreen();
  } else {
    _clockSetEdit->setStep(60);
    _clockSetEdit->setEditing(true);
  }
  _clockSetApply = !_clockSetApply;
}

std::string System::formatTimeClock(long seconds){
  if (_clockMode == CLOCK_MODE_12) return formatTimeHM12(seconds);
  return formatTimeHM24(seconds);
}

void System::resetState(){
  for (uint8_t i = 0; i < MAX_QUEUE_SIZE; i++){
    _FDqueuePins[i] = 0;
    _FDqueueStates[i] = false;
    _FDqueueTimes[i] = 0;
  }
  _FDqueueIndex = -1;
  _counter = 0;
  digitalWrite(_pumpPin, false);
  digitalWrite(_levels[0]->getSolPin(), false);
  digitalWrite(_levels[1]->getSolPin(), false);
  digitalWrite(_levels[2]->getSolPin(), false);
}

void System::resetLEDORCounter(){
  _LEDoverrideCounter = _LEDoverrideTimeout;
}

void System::setScreenSaver(){
  if(_menu->getScreen() == _screenSaver) return;
  _menu->setScreen(_screenSaver);
  _lcd->noBacklight();
}

void System::setStatusMessage(char* message){
  strcpy(_statusMessage, message);
  debug(message);
}

void System::linkLevels(uint8_t level, uint8_t copyFrom){
  _levels[level]->setLEDTiming(_levels[copyFrom]->_LEDonPeriodSeconds,
                               _levels[copyFrom]->_LEDoffPeriodSeconds,
                               _levels[copyFrom]->_LEDcounterSeconds,
                               !_levels[copyFrom]->_LEDstate);
  _levels[level]->setFDTiming(_levels[copyFrom]->_FDperiodSeconds,
                              _levels[copyFrom]->_FDcounterSeconds,
                              true);
}

void System::OR_allLightsOn(){
  _levels[0]->OR_setLED(true);
  _levels[1]->OR_setLED(true);
  _levels[2]->OR_setLED(true);
}

void System::OR_allLightsOff(){
  _levels[0]->OR_setLED(false);
  _levels[1]->OR_setLED(false);
  _levels[2]->OR_setLED(false);
}

void System::OR_revertLights(){
  _levels[0]->OR_revertLED();
  _levels[1]->OR_revertLED();
  _levels[2]->OR_revertLED();
  _LEDoverrideCounter = 0;
}

void System::OR_floodAll30(){
  if (unableToQueue(8)) return;
  resetState();
  addActionToQueue(_levels[0]->getSolPin(), true, SOL_SWITCH_SECONDS);
  addActionToQueue(_levels[1]->getSolPin(), true, SOL_SWITCH_SECONDS);
  addActionToQueue(_levels[2]->getSolPin(), true, SOL_SWITCH_SECONDS);
  addActionToQueue(_pumpPin, true, 30);
  addActionToQueue(_pumpPin, false, SOL_SWITCH_SECONDS);
  addActionToQueue(_levels[2]->getSolPin(), false, SOL_SWITCH_SECONDS);
  addActionToQueue(_levels[1]->getSolPin(), false, SOL_SWITCH_SECONDS);
  addActionToQueue(_levels[0]->getSolPin(), false, SOL_SWITCH_SECONDS);
}

void System::OR_drainAll60(){
  if (unableToQueue(6)) return;
  resetState();
  addActionToQueue(_levels[0]->getSolPin(), true, SOL_SWITCH_SECONDS);
  addActionToQueue(_levels[1]->getSolPin(), true, SOL_SWITCH_SECONDS);
  addActionToQueue(_levels[2]->getSolPin(), true, 60);
  addActionToQueue(_levels[2]->getSolPin(), false, SOL_SWITCH_SECONDS);
  addActionToQueue(_levels[1]->getSolPin(), false, SOL_SWITCH_SECONDS);
  addActionToQueue(_levels[0]->getSolPin(), false, SOL_SWITCH_SECONDS);
}

void System::OR_drainAll600(){
  if (unableToQueue(6)) return;
  resetState();
  addActionToQueue(_levels[0]->getSolPin(), true, SOL_SWITCH_SECONDS);
  addActionToQueue(_levels[1]->getSolPin(), true, SOL_SWITCH_SECONDS);
  addActionToQueue(_levels[2]->getSolPin(), true, 600);
  addActionToQueue(_levels[2]->getSolPin(), false, SOL_SWITCH_SECONDS);
  addActionToQueue(_levels[1]->getSolPin(), false, SOL_SWITCH_SECONDS);
  addActionToQueue(_levels[0]->getSolPin(), false, SOL_SWITCH_SECONDS);
}

void System::OR_enablePump60(){
  if (unableToQueue(2)) return;
  resetState();
  addActionToQueue(_pumpPin, true, 60);
  addActionToQueue(_pumpPin, false, SOL_SWITCH_SECONDS);
}

void System::OR_disablePump(){
  resetState();
}

void System::LEDORTimeout(){
  OR_revertLights();
}

void System::readSensors(){
  _temp = int8_t(readTemp());
  _pH = read_pH();
  _eC = long(readEC());
}

void System::updateInfoScreen(){
  bool update = false;

  // clockString
  std::string padding = _clockMode == CLOCK_MODE_24 ? "   " : ""; 
  char* clockString = (char*)(padding + formatTimeClock(_clockTime)).c_str();
  strcpy(_clockString, clockString);
  if (strcmp(_clockString, _lastClockString) != 0) update = true;
  strcpy(_lastClockString, _clockString);

  // nextFDString
  char* nextFDString;
  if (_FDqueueIndex >= 0){
    nextFDString = "Now";
  } else {

    long nextSeconds = min(min(_levels[0]->_FDcounterSeconds, _levels[1]->_FDcounterSeconds), _levels[2]->_FDcounterSeconds);
    if (nextSeconds < 60) nextFDString = (char*)(std::to_string(nextSeconds) + "s").c_str();
    else nextFDString = (char*)formatTimeHM24(nextSeconds).c_str();
  }
  strcpy(_nextFDString, nextFDString);
  if (strcmp(_nextFDString, _lastNextFDString) != 0) update = true;
  strcpy(_lastNextFDString, _nextFDString);

  // update sensors
  if (_clockTime% SENSOR_REFRESH_RATE == 0){
    readSensors();
    update = true;
  }

  // update menu
  if (update && _menu->getScreen() == _infoScreen){
    _menu->update();
  }
}

void System::completeQueueAction(){
  digitalWrite(_FDqueuePins[0], _FDqueueStates[0]);
  _counter = _FDqueueTimes[0];
  for(uint8_t i = 0; i < _FDqueueIndex; i++){
    if (i == _FDqueueIndex){
      _FDqueuePins[i] = 0;
      _FDqueueStates[i] = false;
      _FDqueueTimes[i] = 0;
    } else {
      _FDqueuePins[i] = _FDqueuePins[i+1];
      _FDqueueStates[i] = _FDqueueStates[i+1];
      _FDqueueTimes[i] = _FDqueueTimes[i+1];
    }
  }
  _FDqueueIndex -= 1;
  if (_FDqueueIndex == -1) resetState();
}

void System::loadSystem(){
  _clockMode = bool(EEPROM.read(0));
  for (uint8_t i = 0; i < 3; i++){
    _levels[i]->loadLevel();
    debug("System Level Loaded");
  }
}

void System::saveSystem(){
  debug("system saved");
  EEPROM.update(0, uint8_t(_clockMode));
  for (uint8_t i = 0; i < 3; i++){
    _levels[i]->saveLevel();
  }
  EEPROM.commit();
}

void System::DB_setHeartbeat(){
  _heartbeat = bool(_debugHeartbeat->getValue() == 1);
}

void System::DB_setHeartbeatVolume(){
  _speaker.setVolume(uint8_t(_debugHeartbeatVolume->getValue()));
}