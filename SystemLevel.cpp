#include "SystemLevel.h"

SystemLevel::SystemLevel(System* system, uint8_t level, uint8_t ledPin, uint8_t solPin){
  _system = system;
  _level = level;
  std::string levelName = "Level " + std::to_string(_level + 1);
  _levelName = new char[levelName.length() + 1];
  strcpy(_levelName, levelName.c_str());
  _ledPin = ledPin;
  _solPin = solPin;
  _enabled = true;

  _LEDonPeriodSeconds = LED_DEFAULT_ON_SECONDS;
  _LEDoffPeriodSeconds = LED_DEFAULT_OFF_SECONDS;
  _LEDstate = LED_DEFAULT_STATE;
  _ORLEDstate = LED_DEFAULT_STATE; 
  _LEDcounterSeconds = LED_DEFAULT_START_SECONDS;

  _FDperiodSeconds = FD_DEFAULT_PERIOD_SECONDS;
  _FDfloodSeconds = FD_DEFAULT_FLOOD_SECONDS;
  _FDdrainSeconds = FD_DEFAULT_DRAIN_SECONDS;
  _FDcounterSeconds = FD_DEFAULT_START_SECONDS;

  setLED(_LEDstate);
}

void SystemLevel::initScheduleMenu(LCDScreen* scheduleScreen){
  _timingScreen = new LCDSubScreen(scheduleScreen, _levelName);
  _timingScreen->setOnEntryClickFunction([this](){resetAllLineVars();});
  
  // -------- General settings --------
  _blankLine = new LCDLine(_timingScreen);
  _headerLine = new LCDLine(_timingScreen, "----- General -----");
  const char* choicesDisabledEnabled[] = {"disabled", "enabled"};
  _enableEdit = new LCDChoiceLine(_timingScreen, 2, (char**)choicesDisabledEnabled, "Activity: ", nullptr, 1);
  
  // -------- Link menu --------
  _linkScreen = new LCDSubScreen(_timingScreen, "Link to other level");
  if (_level != 0){
      linkLine1 = new LCDLine(_linkScreen, "Link with level 1");
      linkLine1->setTarget(scheduleScreen);
      linkLine1->setClickFunction([this](){linkLevel1();});
  }
  if (_level != 1){
      linkLine2 = new LCDLine(_linkScreen, "Link with level 2");
      linkLine2->setTarget(scheduleScreen);
      linkLine2->setClickFunction([this](){linkLevel2();});
  }
  if (_level != 2){
      linkLine3 = new LCDLine(_linkScreen, "Link with level 3");
      linkLine3->setTarget(scheduleScreen);
      linkLine3->setClickFunction([this](){linkLevel3();});
  }

  // -------- LED settings --------
  _blankLine = new LCDLine(_timingScreen);
  _headerLine = new LCDLine(_timingScreen, "---- Lighting ----");
  _LEDonPeriodEdit = new LCDEditLine(_timingScreen, "On period:  ", _LEDonPeriodSeconds, LED_MIN_PERIOD_SECONDS, LED_MAX_PERIOD_SECONDS, LED_PERIOD_STEP_SECONDS, formatTimeDecimalHours, [this](){LEDonPeriodUpdate();});
  _LEDoffPeriodEdit = new LCDEditLine(_timingScreen, "Off period: ", _LEDoffPeriodSeconds, LED_MIN_PERIOD_SECONDS, LED_MAX_PERIOD_SECONDS, LED_PERIOD_STEP_SECONDS, formatTimeDecimalHours, [this](){LEDoffPeriodUpdate();});
  _LEDstartDelayEdit = new LCDEditLine(_timingScreen, "Sched for: ", 0, 0, SECONDS_IN_ONE_DAY, FD_DELAY_STEP_SECONDS, [this](long seconds) -> std::string{return this->formatTimeFuture(seconds);});
  const char* choicesoffOn[] = {"off", "on"};
  _LEDresetStateEdit = new LCDChoiceLine(_timingScreen, 2, (char**)choicesoffOn, "Start with LED: ", nullptr, 1);

  // -------- FD settings --------
  _blankLine = new LCDLine(_timingScreen);
  _headerLine = new LCDLine(_timingScreen, "-- Flood & Drain --");
  _FDperiodEdit = new LCDEditLine(_timingScreen, "Cycle period: ", _FDperiodSeconds, FD_MIN_PERIOD_SECONDS, FD_MAX_PERIOD_SECONDS, FD_PERIOD_STEP_SECONDS, formatTimeDecimalHours);
  _FDstartDelayEdit = new LCDEditLine(_timingScreen, "Sched for: ", 0, 0, SECONDS_IN_ONE_DAY, LED_DELAY_STEP_SECONDS, [this](long seconds) -> std::string{return this->formatTimeFuture(seconds);});
  const char* choicesNoYes[] = {"no", "yes"};
  _FDresetStateEdit = new LCDChoiceLine(_timingScreen, 2, (char**)choicesNoYes, "Immediate F/D: ", nullptr, 0);

  // -------- Update settings --------
  _blankLine = new LCDLine(_timingScreen);
  _headerLine = new LCDLine(_timingScreen, "-- Save & Update --");
  _updateLEDLine = new LCDLine(_timingScreen, "LED schedule only");
  _updateFDLine = new LCDLine(_timingScreen, "F/D schedule only");
  _updateAllLine = new LCDLine(_timingScreen, "Both schedules");
  _updateLEDLine->setTarget(scheduleScreen);
  _updateFDLine->setTarget(scheduleScreen);
  _updateAllLine->setTarget(scheduleScreen);
  _updateLEDLine->setClickFunction([this](){updateLEDTiming();});
  _updateFDLine->setClickFunction([this](){updateFDTiming();});
  _updateAllLine->setClickFunction([this](){updateAllTiming();});

  resetAllLineVars();
}

void SystemLevel::setFDCycleTimings(long floodDuration, long drainDuration){
  _FDfloodSeconds = floodDuration;
  _FDdrainSeconds = drainDuration;
}

void SystemLevel::setLED(bool state){
  if (!_enabled) return;
  _LEDstate = state;
  _ORLEDstate = state;
  digitalWrite(_ledPin, _LEDstate);
}

void SystemLevel::toggleLED(){
  setLED(!_LEDstate);
}

void SystemLevel::setLEDTiming(long onPeriod, long offPeriod, long startDelay, bool startState){
  _LEDonPeriodSeconds = onPeriod;
  _LEDoffPeriodSeconds = offPeriod;
  setLED(!startState);
  _LEDcounterSeconds = startDelay;
  resetLEDLineVars();
}

void SystemLevel::setFDTiming(long cyclePeriod, long startDelay, bool startWithFD){
  _FDperiodSeconds = cyclePeriod;
  if (startWithFD) _FDcounterSeconds = startDelay;
  else _FDcounterSeconds = startDelay + _FDperiodSeconds;
  resetFDLineVars();
}

uint8_t SystemLevel::getLEDPin(){
  return _ledPin;
}

uint8_t SystemLevel::getSolPin(){
  return _solPin;
}

void SystemLevel::resetLEDLineVars(){
  _LEDonPeriodEdit->setValue(_LEDonPeriodSeconds);
  _LEDoffPeriodEdit->setValue(_LEDoffPeriodSeconds);
  _LEDstartDelayEdit->setValue(0);
  _LEDresetStateEdit->setValue(1);
}

void SystemLevel::resetFDLineVars(){
  _FDperiodEdit->setValue(_FDperiodSeconds);
  _FDstartDelayEdit->setValue(0);
  _FDresetStateEdit->setValue(1);
}

void SystemLevel::resetAllLineVars(){
  resetLEDLineVars();
  resetFDLineVars();
}

void SystemLevel::OR_setLED(bool state){
  _ORLEDstate = state;
  digitalWrite(_ledPin, _ORLEDstate);
  _system->resetLEDORCounter();
}

void SystemLevel::OR_toggleLED(){
  OR_setLED(!_ORLEDstate);
}

void SystemLevel::OR_revertLED(){
  setLED(_LEDstate);
}

void SystemLevel::fullFDCycle(){
  if (_system->unableToQueue(4)) return;
  _system->addActionToQueue(_solPin, true, SOL_SWITCH_SECONDS);
  _system->addActionToQueue(_system->getPumpPin(), true, _FDfloodSeconds);
  _system->addActionToQueue(_system->getPumpPin(), false, _FDdrainSeconds);
  _system->addActionToQueue(_solPin, false, SOL_SWITCH_SECONDS);
}

void SystemLevel::OR_fullFDCycle(){
  _system->resetState();
  fullFDCycle();
}

void SystemLevel::OR_flood15(){
  if (_system->unableToQueue(4)) return;
  _system->resetState();
  _system->addActionToQueue(_solPin, true, SOL_SWITCH_SECONDS);
  _system->addActionToQueue(_system->getPumpPin(), true, 15);
  _system->addActionToQueue(_system->getPumpPin(), false, SOL_SWITCH_SECONDS);
  _system->addActionToQueue(_solPin, false, SOL_SWITCH_SECONDS);
}

void SystemLevel::OR_drain60(){
  if (_system->unableToQueue(2)) return;
  _system->resetState();
  _system->addActionToQueue(_solPin, true, 60);
  _system->addActionToQueue(_solPin, false, SOL_SWITCH_SECONDS);
}

void SystemLevel::OR_drain600(){
  if (_system->unableToQueue(2)) return;
  _system->resetState();
  _system->addActionToQueue(_solPin, true, 600);
  _system->addActionToQueue(_solPin, false, SOL_SWITCH_SECONDS);
}

void SystemLevel::OR_resetFD(){
  _FDcounterSeconds = _FDperiodSeconds;
}

void SystemLevel::OR_resetLEDOn(){
  setLED(true);
  _LEDcounterSeconds = _LEDoffPeriodSeconds;
}

void SystemLevel::OR_resetLEDOff(){
  setLED(false);
  _LEDcounterSeconds = _LEDonPeriodSeconds;
}

void SystemLevel::linkLevel1(){_system->linkLevels(_level, 0);}
void SystemLevel::linkLevel2(){_system->linkLevels(_level, 1);}
void SystemLevel::linkLevel3(){_system->linkLevels(_level, 2);}

void SystemLevel::LEDonPeriodUpdate(){
  _LEDoffPeriodSeconds = SECONDS_IN_ONE_DAY - _LEDonPeriodEdit->getValue();
  _LEDoffPeriodEdit->setValue(_LEDoffPeriodSeconds);
}

void SystemLevel::LEDoffPeriodUpdate(){
  _LEDonPeriodSeconds = SECONDS_IN_ONE_DAY - _LEDoffPeriodEdit->getValue();
  _LEDonPeriodEdit->setValue(_LEDonPeriodSeconds);
}

void SystemLevel::updateLEDTiming(bool save){
  long interval = _LEDstartDelayEdit->getValue();
  long delay;
  if (interval == 0) delay = 0;
  else{
    long now = _system->getTime();
    long futureTime = ((interval + now) / LED_DELAY_STEP_SECONDS) * LED_DELAY_STEP_SECONDS;
    delay = futureTime - now;
  }
  checkEnabled();
  setLEDTiming(_LEDonPeriodEdit->getValue(),
               _LEDoffPeriodEdit->getValue(),
               delay,
               _LEDresetStateEdit->getValue() == 1);
  if (save) _system->saveSystem();
}

void SystemLevel::updateFDTiming(bool save){
  long interval = _FDstartDelayEdit->getValue();
  long delay;
  if (interval == 0) delay = 0;
  else{
    long now = _system->getTime();
    long futureTime = ((interval + now) / LED_DELAY_STEP_SECONDS) * LED_DELAY_STEP_SECONDS;
    delay = futureTime - now;
  }
  checkEnabled();
  setFDTiming(_FDperiodEdit->getValue(),
              delay,
              _FDresetStateEdit->getValue() == 1);
  if (save) _system->saveSystem();
}

void SystemLevel::updateAllTiming(){
  updateLEDTiming(false);
  updateFDTiming(false);
  _system->saveSystem();
}

void SystemLevel::checkEnabled(){
  if (_enableEdit->getValue() == 1) _enabled = true;
  else {
    _enabled = false;
    setLED(false);
  }
}

void SystemLevel::tick(long secondsPassed){
  if (!_enabled) return;
  // FD
  _FDcounterSeconds -= secondsPassed;
  if (_FDcounterSeconds <= 0){
    _FDcounterSeconds += _FDperiodSeconds;
    fullFDCycle();
  }
  // LED
  _LEDcounterSeconds -= secondsPassed;
  if (_LEDcounterSeconds <= 0){
    if (_LEDstate) _LEDcounterSeconds += _LEDoffPeriodSeconds;
    else _LEDcounterSeconds += _LEDonPeriodSeconds;
    toggleLED();
  }
}

std::string SystemLevel::formatTimeFuture(long seconds){
  if (seconds == 0) return "Now";
  long now = (_system->getTime() / 60) * 60;
  long future = ((seconds + now) / FD_DELAY_STEP_SECONDS) * FD_DELAY_STEP_SECONDS;
  return _system->formatTimeClock(future);
}

void SystemLevel::loadLevel(){
  int address = SYSTEM_ADDRESS_WIDTH + _level * SYSTEM_LEVEL_ADDRESS_WIDTH;
  _LEDonPeriodSeconds = long(EEPROM.read(address++)) * 1800;
  _LEDoffPeriodSeconds = long(EEPROM.read(address++)) * 1800;
  _FDperiodSeconds = long(EEPROM.read(address++)) * 1800;
  resetAllLineVars();
}

void SystemLevel::saveLevel(){
  int address = SYSTEM_ADDRESS_WIDTH + _level * SYSTEM_LEVEL_ADDRESS_WIDTH;
  EEPROM.update(address++, uint8_t(_LEDonPeriodSeconds / 1800));
  EEPROM.update(address++, uint8_t(_LEDoffPeriodSeconds / 1800));
  EEPROM.update(address++, uint8_t(_FDperiodSeconds / 1800));
}
