//Datasheet : http://www.ti.com/lit/ds/symlink/dac7562.pdf
#ifndef Electrode_h
#define Electrode_h
#include "Arduino.h"
#include <SPI.h>

class Electrode{
public:
  volatile uint8_t *CS_OutReg;
  volatile uint8_t *EN_OutReg;
  volatile uint8_t *FLIP_OutReg;

  uint8_t CS_pin;
  uint8_t EN_pin;
  uint8_t FLIP_pin;

  bool isPositive = true;


  Electrode(volatile uint8_t *dac_CS_dirReg,volatile uint8_t *dac_CS_OutReg, uint8_t dac_CS_pin,
            volatile uint8_t *switch_enable_dirReg,volatile uint8_t *switch_enable_outputReg, uint8_t switch_enable_pin,
            volatile uint8_t *switch_flip_dirReg,volatile uint8_t *switch_flip_outputReg, uint8_t switch_flip_pin);
  void begin();
  void setValue(uint16_t value);
  void positive();
  void negative();
  void on();
  void off();
private:
  void transfer(uint8_t byteOne, uint8_t byteTwo, uint8_t byteThree);
};
#endif
