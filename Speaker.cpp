#include "Speaker.h"

int Speaker::_freq = NOTE_MAX;
uint8_t Speaker::_volume = 255;
bool Speaker::_playing = false;
volatile bool Speaker::_waveState = false;

Speaker::Speaker() {

  GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                      GCLK_GENCTRL_GENEN |         // Enable GCLK 4
                      GCLK_GENCTRL_SRC_DFLL48M |   // Set the clock source to 48MHz DFLL
                      GCLK_GENCTRL_ID(4);          // Set clock source on GCLK 4
  while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) |          // Divide the 48MHz signal by 1: 48MHz/1 = 48MHz
                     GCLK_GENDIV_ID(4);            // Set division on Generic Clock Generator (GCLK) 4
  while (GCLK->STATUS.bit.SYNCBUSY);

  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |         // Route GCLK4 to TCC0 and TCC1
                      GCLK_CLKCTRL_GEN_GCLK4 |     
                      GCLK_CLKCTRL_ID_TCC0_TCC1;
  while (GCLK->STATUS.bit.SYNCBUSY);

  

  // Set the PWM frequency to 62,500 Hz
  // The prescaler value of 1 is used to obtain the maximum frequency
  TCC0->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV1 | TCC_CTRLA_ENABLE;
  while (TCC0->SYNCBUSY.bit.ENABLE);
  TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM; // Set the timer to normal PWM mode
  while(TCC0->SYNCBUSY.bit.WAVE);
  TCC0->PER.reg = 255; // Set the period to 255 (8-bit resolution)
  while(TCC0->SYNCBUSY.bit.PER);
  TCC0->CC[3].reg = 0; // Set the duty cycle to 00% (0/255)
  while(TCC0->SYNCBUSY.bit.CC2);
  TCC0->CTRLBSET.reg = TCC_CTRLBSET_LUPD; // Update the CTRLB register to apply changes
  while(TCC0->SYNCBUSY.bit.CTRLB); // Wait for CTRLB register update

  PORT->Group[PORTA].DIRSET.reg = PORT_PA21;           // Set pin as output
  PORT->Group[PORTA].OUTCLR.reg = PORT_PA21;           // Set pin to low
  PORT->Group[PORTA].PINCFG[21].bit.PMUXEN = 1;        // enable multiplexing on D10 (PA21)
  PORT->Group[PORTA].PMUX[10].reg = PORT_PMUX_PMUXO_F; // route CC[3] to D10 (PA21)
  
  

  // Set tone frequency timer
  TCC1->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV1 | TCC_CTRLA_ENABLE;
  while (TCC1->SYNCBUSY.bit.ENABLE);
  TCC1->INTENSET.reg = TCC_INTENSET_MC0; // Enable interrupt on match channel 0
  TCC1->WAVE.reg |= TCC_WAVE_WAVEGEN_NFRQ; // Set the timer to normal PWM mode
  while(TCC1->SYNCBUSY.bit.WAVE);
  TCC1->PER.reg = F_CPU / (2 * Speaker::_freq); // Set the period to generate the desired frequency
  while(TCC1->SYNCBUSY.bit.PER);
  TCC1->CTRLBSET.reg = TCC_CTRLBSET_LUPD; // Update the CTRLB register to apply changes
  while(TCC1->SYNCBUSY.bit.CTRLB); // Wait for CTRLB register update

  NVIC_SetPriority(TCC1_IRQn, 0);
  NVIC_EnableIRQ(TCC1_IRQn);

  stop();
}


void TCC1_Handler() {
  // Clear the interrupt flag
  TCC1->INTFLAG.reg = TCC_INTFLAG_MC0;

  // Toggle the tone state to generate the square wave
  Speaker::_waveState = !Speaker::_waveState;
  if (Speaker::_waveState){
    TCC0->CC[3].reg = Speaker::_volume; // Set the duty cycle to volume
  } else {
    TCC0->CC[3].reg = 0; // Set the duty cycle to 0% (0/255)
  }
}


void Speaker::setVolume(uint8_t vol){
  Speaker::_volume = vol;
}


void Speaker::setFrequency(int freq){
  Speaker::_freq = freq;
  // Update the timer period to generate the new frequency
  TCC1->PER.reg = F_CPU / (2 * _freq);
  // Set the duty cycle to generate a square wave
  TCC1->CC[0].reg = TCC1->PER.reg / 2;
}


void Speaker::start(){
  Speaker::_playing = true;
  TCC1->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV1 | TCC_CTRLA_ENABLE;
  while (TCC1->SYNCBUSY.bit.ENABLE);
}


void Speaker::stop(){
  Speaker::_playing = false;
  TCC1->CTRLA.reg = 0;
  while (TCC1->SYNCBUSY.bit.ENABLE);
}


void Speaker::tone(int freq, int ms, uint8_t vol){
  setFrequency(freq);
  setVolume(vol);
  start();
  if(ms > 0){
    delay(ms);
    stop();
  }
}


void Speaker::startUpTones(){
  tone(NOTE_C5, 300);
  tone(NOTE_A5, 300);
  tone(NOTE_F5, 700);
  delay(1000);
}


void Speaker::beep(){
  tone(NOTE_MAX, 50, _volume);
}