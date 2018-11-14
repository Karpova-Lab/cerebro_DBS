
#ifndef Radio_h
#define Radio_h
#include "Arduino.h"
#include <SPI.h>
#include <RFM69_ATC.h>     //get it here: https://www.github.com/lowpowerlab/rfm69

//*********************************************************************************************
#define BASESTATION   1
#define CEREBRO       2
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
#define FREQUENCY     RF69_915MHZ
#define IS_RFM69HW_HCW  //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!
//*********************************************************************************************
//Auto Transmission Control - dials down transmit power to save battery
//Usually you do not need to always transmit at max output power
//By reducing TX power even a little you save a significant amount of battery power
//This setting enables this gateway to work with remote nodes that have ATC enabled to
//dial their power down to only the required level (ATC_RSSI)
#define ATC_RSSI      -80

typedef struct { // 16 bytes
  uint16_t msgCount;   
  uint16_t startDelay;
  uint8_t posAmp;
  uint16_t posDur;
  uint8_t negAmp;
  uint16_t negDur;
  uint16_t freq;
  uint32_t trainDur;
} WaveformData;

typedef struct { // 5 bytes
  uint16_t msgCount;    
  uint8_t variable;
  uint16_t value;
} IntegerPayload;

typedef struct { // 4 bytes
  uint16_t msgCount;  
  byte serialNumber;
  byte firmware;
} Info;

class Radio: public RFM69_ATC{
public:
  Radio(uint8_t slaveSelectPin, uint8_t interruptPin):
  RFM69_ATC(slaveSelectPin, interruptPin,true, digitalPinToInterrupt(interruptPin)){
  }

  void radioSetup(uint8_t nodeID,bool autoPower, uint8_t networkID);
};
#endif
