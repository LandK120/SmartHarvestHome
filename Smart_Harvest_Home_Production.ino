#include "System.h"
#include "RotaryEncoder.h" // library (Matthias Hertel)

System* sys;
RotaryEncoder encoder(PIN_ENC_CLOCK, PIN_ENC_DATA);

long currentTime = millis();
long clickTime = millis();
long tickTime = millis();

bool clickState = false;
bool clickOccured = false;
bool downOccured = false;
bool upOccured = false;
bool holding = false;
bool ignoreRelease = false;

int input;

void onRotate();
void onSwitch();

void setup() {
  if (SERIAL_LCD || SERIAL_ENCODER) Serial.begin(9600);
  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_SOL_1, OUTPUT);
  pinMode(PIN_SOL_2, OUTPUT);
  pinMode(PIN_SOL_3, OUTPUT);
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(PIN_LED_3, OUTPUT);
  pinMode(PIN_ENC_DATA, INPUT_PULLUP);
  pinMode(PIN_ENC_CLOCK, INPUT_PULLUP);
  pinMode(PIN_ENC_SWITCH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_CLOCK), onRotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_DATA), onRotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_SWITCH), onClick, CHANGE);
  sys = new System();
}

void loop() {
  currentTime = millis();

  // calling tick too often will corrupt rtc
  if (currentTime - tickTime >= TICK_MS){
    sys->tick(currentTime);
    tickTime = currentTime;
  }

  if (clickOccured && currentTime - clickTime >= ENCODER_DEBOUNCE_MS){
    if (clickState == false){
      holding = true;
    } else if (holding) { // only trigger if releasing switch and the switch was considered held
      if (!ignoreRelease) sys->encoderClick();
      ignoreRelease = false;
      holding = false;
    }
    clickOccured = false;
  }
  if (holding && currentTime - clickTime >= ENCODER_HOLD_MS){
    ignoreRelease = true; // prevent release of holding to be considered a secondary regular click
    holding = false;
    sys->encoderHold();
  }
  if (downOccured){
    sys->encoderDown();
    downOccured = false;
  }
  if (upOccured){
    sys->encoderUp();
    upOccured = false;
  }

  if (SERIAL_ENCODER && Serial.available() > 0){
    input = Serial.read();
    switch(input){
      case 97: //"a"
        sys->encoderDown();
        break;
      case 100: //"d"
        sys->encoderUp();
        break;
      case 115: //"s"
        sys->encoderClick();
        break;
      case 119: //"w"
        sys->encoderHold();
        break;
      default:
        break;
    }
  }
}

void onClick(){
  clickTime = millis();
  clickState = digitalRead(PIN_ENC_SWITCH);
  clickOccured = true;
}

void onRotate(){
  encoder.tick();
  RotaryEncoder::Direction dir = encoder.getDirection();
  if (dir == RotaryEncoder::Direction::COUNTERCLOCKWISE) downOccured = true;
  else if (dir == RotaryEncoder::Direction::CLOCKWISE) upOccured = true;
}


