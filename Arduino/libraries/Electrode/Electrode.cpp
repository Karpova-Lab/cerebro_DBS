//DAC datasheet : http://www.ti.com/lit/ds/symlink/dac7562.pdf
//Switch datasheet: https://www.analog.com/media/en/technical-documentation/data-sheets/ADG1433_1434.pdf

#include "Electrode.h"
Electrode::Electrode(volatile uint8_t *dac_CS_dirReg,volatile uint8_t *dac_CS_OutReg, uint8_t dac_CS_pin,
                     volatile uint8_t *switch_enable_dirReg,volatile uint8_t *switch_enable_outputReg, uint8_t switch_enable_pin,
                     volatile uint8_t *switch_flip_dirReg,volatile uint8_t *switch_flip_outputReg, uint8_t switch_flip_pin){
  
  //Setup DAC
  CS_OutReg = dac_CS_OutReg;
  CS_pin = dac_CS_pin;
  *dac_CS_dirReg |= (1<<CS_pin);        //set chip select pin as output
  *CS_OutReg |= (1<<CS_pin);            //Deselect the DAC by outputting Chip Select HIGH (pin is active low)

  // setup ADG1433 
  EN_OutReg = switch_enable_outputReg;
  EN_pin = switch_enable_pin;
  *switch_enable_dirReg |= (1<<EN_pin); //set chip switch enable pin as output
  on();            //disable the switch. Electrodes start of disconnected

  FLIP_OutReg = switch_flip_outputReg;
  FLIP_pin = switch_flip_pin;
  *switch_flip_dirReg |= (1<<FLIP_pin); //set switch flip pin as output
  positive();        //starting polarity for electrode
}



void Electrode::transfer(uint8_t byteOne, uint8_t byteTwo, uint8_t byteThree){
  //send data to DAC
  cli(); //disable interrupts
  SPI.beginTransaction(SPISettings(50000000, MSBFIRST, SPI_MODE0));
  *CS_OutReg &= ~(1<<CS_pin);  //Select DAC chip by outputting chip select LOW  
  SPI.transfer(byteOne);                           
  SPI.transfer(byteTwo);                            
  SPI.transfer(byteThree);                          
  *CS_OutReg |= (1<<CS_pin);   //Deselect DAC chip by outputting chip select LOW  
  SPI.endTransaction();
  sei(); //enable interrupts
}

void Electrode::begin() {
  transfer(255, 0, 1); // enable interal reference
  transfer(2,0,3); //set gain=1
  transfer(B00100000,0,B00000011); //power up DAC-A and DAC-B
  off();
}

void Electrode::setValue(uint16_t value) {
  transfer(31, value>>8, value & 255); //Write to DAC-A and DAC-B input register and update all DACs
}

void Electrode::positive(){
  *FLIP_OutReg |= (1<<FLIP_pin);
  isPositive =  true;
}

void Electrode::negative(){
  *FLIP_OutReg &= ~(1<<FLIP_pin);
  isPositive =  false;
}

void Electrode::on(){
  *EN_OutReg &= ~(1<<EN_pin);
}

void Electrode::off(){
  *EN_OutReg |= (1<<EN_pin);
}