/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MIT License

Copyright (c) 2018 Andy S. Lustig

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
const uint8_t VERSION = 0; // 2018-11-14

#include <EEPROM.h>
#include <Electrode.h>
#include <SparkFunBQ27441.h>      //https://github.com/sparkfun/SparkFun_BQ27441_Arduino_Library
#include <Radio.h>                //https://github.com/LowPowerLab/RFM69
#include <avr/wdt.h>
#include <util/delay.h>

#define SERIAL_NUMBER_ADDRESS 0
#define WAVEFORM_ADDRESS 1
#define MISSING_ARRAY_ADDRESS 17

WaveformData waveform;
IntegerPayload integerMessage;  
Info cerebroInfo;

uint16_t meterVal = 0;

const uint8_t amberLED = A0; //32u4 pin 36
const uint8_t greenLED = 12; //32u4 pin 26
const uint8_t userBTN = 6; //32u4 pin 27
Electrode stim(&DDRB, &PORTB, 0,  //DAC SYNC pin is the chip select pin. p8/PB0/D17.
               &DDRB, &PORTB, 7,  //Switch enable pin. p12/PB7/D11
               &DDRF, &PORTF, 0); //Switch flip pin. p41/PF0/A5

uint8_t channel;
Radio radio(7,1); //slave select pin, interrupt pin
uint16_t msgCount = 0;
uint16_t missedCount = 0;
uint16_t trigCount = 0;
uint8_t batteryUpdateFrequency = 10;
bool reportBatteryFlag = false;

void setup() {
  SPI.begin();
  stim.begin();
  Serial.begin(115200);

  // Initialize waveform
  EEPROM.get(WAVEFORM_ADDRESS,waveform);

  // UI setup
  pinMode(userBTN,INPUT_PULLUP);
  pinMode(greenLED,OUTPUT);
  pinMode(amberLED,OUTPUT);
  digitalWrite(amberLED,HIGH);

  //*** Battery Monitor ***//
  if (!lipo.begin()){
	// If communication fails, print an error message and loop forever.
    Serial.println("Error: Unable to communicate with BQ27441.");
    while(1){ //blink error
      digitalWrite(amberLED,HIGH);
      delay(200);
      digitalWrite(amberLED,LOW);
      delay(200);
    }
  }
  Serial.println("Connected to BQ27441!");
  lipo.setCapacity(400);
  while(lipo.soc()==0){
    delay(1);
    //wait;
  }

  //*** Radio ***//
  EEPROM.get(SERIAL_NUMBER_ADDRESS,channel); // we use our unique serial number as our radio communication channel
  radio.radioSetup(CEREBRO,true,channel); //nodeID, autopower on;
  integerMessage.variable = 'Y';
  integerMessage.value = millis();
  if (radio.sendWithRetry(BASESTATION, (const void*)(&integerMessage), sizeof(integerMessage),3,250)){
    Serial.println("Connected to Base Station");
  }
  else{
    Serial.println("Failed to Connect to Base Station"); 
    //turn on green LED?
  }
  printChannel();
  reportVersion();
  digitalWrite(amberLED,LOW);
}

void loop() {
  //
  if (!digitalRead(userBTN)){ //user button is pushed
    uint16_t holdCount = 0;
    uint16_t holdThresh = 5000; //need to hold the button for 5 seconds
    digitalWrite(greenLED,HIGH);
    while(!digitalRead(userBTN) && holdCount < holdThresh){
      delay(1);
      holdCount++;
    }
    if (holdCount >= holdThresh) { //Button was held down for 5 seconds
      //Reset settings that are in memory
      EEPROM.update(SERIAL_NUMBER_ADDRESS, 0);
      waveform = {0,0,150,60,150,60,125,60000};
      EEPROM.put(WAVEFORM_ADDRESS,waveform);  //save new waveform to memory

      //blink to indicate button was held long enough and reset has occured
      uint8_t blinkDelay = 85;
      digitalWrite(greenLED,LOW);
      delay(blinkDelay);
      digitalWrite(greenLED,HIGH);
      delay(blinkDelay);
      digitalWrite(greenLED,LOW);
      delay(blinkDelay);
      digitalWrite(greenLED,HIGH);
      delay(blinkDelay);

      //restart MCU so it will 
      Serial.println("Restarting...");
      wdt_enable(WDTO_15MS);  // turn on the WatchDog timer
      while(1){}              // do nothing and wait for the reset
    }
    else{
      digitalWrite(greenLED,LOW);
    }
  }
  if (Serial.available()){
    char msg = Serial.read();
    if (msg=='?'){
      printChannel();
      Serial.print("Version: ");
      Serial.println(VERSION);

    }
  }
  //check for any received packets
  if (radio.receiveDone()){
    uint8_t dlay = 80;
    if (radio.DATALEN==1){ //received a command or a request for data
      sendACK();
      switch (radio.DATA[0]){
        case 'o':
          stim.positive();break;
        case 'n':
          stim.negative();break;
        case 'B':
          reportBattery();break;
        case 'N':
          msgCount = 0;
          missedCount = 0;
          reportVersion();
          delay(dlay);
          reportWaveform();
          delay(dlay);
          reportBattery();break;
        case '+':
          testStim(waveform.posAmp);break;
        case '0':
          stim.off();break;
        default:
          Serial.print("received '");Serial.print(radio.DATA[0]);
          Serial.println("'. Command not recognized");break;
      }
    }
    else if (radio.DATALEN == sizeof(waveform)){ //received a waveform data
      sendACK();
      msgCount++;
      waveform = *(WaveformData*)radio.DATA;  //update waveform
      EEPROM.put(WAVEFORM_ADDRESS,waveform);  //save new waveform to memory
      reportWaveform();
    }
    else if (radio.DATALEN == sizeof(integerMessage)){ //received a variable update
      sendACK();
      integerMessage = *(IntegerPayload*)radio.DATA;
      switch (integerMessage.variable){
        case 'T':
          trigCount++;
          checkForMiss();
          testStim(waveform.posAmp);
          stimulate(waveform.startDelay,
                    waveform.posAmp, waveform.posDur,
                    waveform.negAmp, waveform.negDur, 
                    waveform.freq, 
                    waveform.trainDur);
          break;
        case 'S': // Receiving a new Cerebro S/N
          EEPROM.update(SERIAL_NUMBER_ADDRESS, integerMessage.value);
          reportVersion();
          Serial.print("New Serial:");
          Serial.println(integerMessage.value);
          Serial.println("Restarting...");
          wdt_enable(WDTO_15MS);  // turn on the WatchDog timer
          while(1){}              // do nothing and wait for the reset
          break;
        case 'M':
          checkForMiss();
          printMissed();
          break;
      }
    }
    else{
      Serial.println("Unexpected Data size received");
    }
  }
}

void sendACK(){
  if (radio.ACKRequested()){
    radio.sendACK();
  }
}
 
void checkForMiss(){
  msgCount++;
  if (trigCount%batteryUpdateFrequency==0){
    reportBatteryFlag = true;
  }
  if (msgCount!=integerMessage.value){
    for (msgCount; msgCount<integerMessage.value; msgCount++){
      EEPROM.put(MISSING_ARRAY_ADDRESS + 2*missedCount,msgCount);
      missedCount++;
    }
  }
}

void printMissed(){
  reportBattery();
  delay(200);
  uint16_t missed;
  // Serial.print("missed,");Serial.println(missedCount);
  integerMessage.variable = 'M';
  integerMessage.value = missedCount;
  if (radio.sendWithRetry(BASESTATION, (const void*)(&integerMessage), sizeof(integerMessage),3,250)){
    // Serial.print("Sent");Serial.println(missedCount);
  }
  else{
    // Serial.println("Sending again");
  }
  integerMessage.variable = 'm';
  for (int i = 0; i <missedCount; i++ ){
    EEPROM.get(MISSING_ARRAY_ADDRESS+2*i,missed);
    integerMessage.value = (int)missed;
    if (radio.sendWithRetry(BASESTATION, (const void*)(&integerMessage), sizeof(integerMessage),3,250)){
      // Serial.println(missed);
    }
    else{
      // Serial.println("Sending again");
    }
  }
  // Serial.println("done");
}

void reportVersion(){
  EEPROM.get(SERIAL_NUMBER_ADDRESS,cerebroInfo.serialNumber);
  cerebroInfo.firmware = VERSION;
  cerebroInfo.msgCount = msgCount;
  if (radio.sendWithRetry(BASESTATION, (const void*)(&cerebroInfo), sizeof(cerebroInfo),4,20)){
    // Serial.println("Version sent successfully");
  }
  else{
    // Serial.println("Error sending Version");
  }
}

void reportWaveform(){
  waveform.msgCount = msgCount;
  if (radio.sendWithRetry(BASESTATION, (const void*)(&waveform), sizeof(waveform),4,20)){
    // Serial.println("Waveform sent successfully");
  }
  else{
    // Serial.println("Error sending Waveform");
  }
}

void reportBattery(){
  integerMessage.variable = 'B';
  integerMessage.value = lipo.soc();
  integerMessage.msgCount = msgCount;
  if (radio.sendWithRetry(BASESTATION, (const void*)(&integerMessage), sizeof(integerMessage),2,20)){
    //
  }
  else{
    // Serial.println("Battery info send fail");
  }
}

void printChannel(){
  Serial.print("Channel: ");
  Serial.println(channel);
}

void stimulate(uint16_t startDelay, uint16_t posAmplitude, uint16_t posDuration, uint16_t negAmplitude, uint16_t negDuration, uint16_t freq, uint16_t trainDur )
{
  const uint8_t set_and_switch_delay = 28;
  posAmplitude = int(posAmplitude*19.69) << 4; //current output range is 0-208µA, DAC resolution is 12bit. 2^12/208 = 19.69. Shift to the left becauase the we're using 12bit DAC as opposed to 16bit
  negAmplitude = int(negAmplitude*19.69) << 4; 

  stim.setValue(posAmplitude << 4);
  float interDelay = 1000000.0 / freq - (posDuration+negDuration+set_and_switch_delay);
  Serial.println(interDelay);
  delay(startDelay);
  uint32_t clockStart = millis();
  while ((millis() - clockStart) < trainDur)
  {
    
    stim.setValue(posAmplitude);
    stim.positive();
    stim.on();
    delayMicroseconds(posDuration - set_and_switch_delay);
    stim.setValue(negAmplitude);
    stim.negative();
    delayMicroseconds(negDuration);
    stim.off();
    delayMicroseconds(interDelay);
  }
}

void testStim(uint16_t amplitude)
{
  stim.on();
  Serial.print(millis());
  if (!stim.isPositive)
  {
    Serial.print(" (Positive) ");
    stim.positive();
  }
  else
  {
    Serial.print(" (Negative) ");
    stim.negative();
  }
  Serial.println(amplitude);
  amplitude *= 19.69;
  stim.setValue(amplitude << 4);
}