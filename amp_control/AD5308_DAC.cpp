#include <Arduino.h>
#include "AD5308_DAC.h"
#include "SC18IS602B.h"   // https://github.com/maxgerhardt/SC18IS602B-arduino

const uint8_t HCT259addr[]={ HCT259_S0, HCT259_S1, HCT259_S2};

SC18IS602B spiBridge(-1,1,0,0);

uint8_t slaveNum = 3;
uint8_t SPImode = SC18IS601B_SPIMODE_2;
uint8_t SPIfrequency = SC18IS601B_SPICLK_1843_kHz;

void changeHCT259pin( uint8_t pin, uint8_t state){
    for ( uint8_t i=0; i<=2; i++ ){
      spiBridge.writeGPIO(HCT259addr[i], (pin & (1<< i) ? HIGH : LOW ) );
    }
    digitalWrite(HCT259dPin, state);
    delayMicroseconds(10);
    digitalWrite(HCT259lePin, LOW);
    delayMicroseconds(10);
    digitalWrite(HCT259lePin, HIGH);
    delayMicroseconds(10);
}

void defaultHCT259(){
  for ( uint8_t i=0; i<=6; i++ ){
    changeHCT259pin( i, HIGH);
  }
  changeHCT259pin( HCT259_WE, LOW);
}

void setupDAC(){
  spiBridge.begin();
  for ( uint8_t i=0; i<=2; i++){
    spiBridge.enableGPIO(HCT259addr[i], true);
    spiBridge.setupGPIO(HCT259addr[i], SC18IS601B_GPIO_MODE_PUSH_PULL);
  }
  
  pinMode(HCT259dPin, OUTPUT);
  pinMode(HCT259lePin, OUTPUT);
  digitalWrite(HCT259dPin, LOW);
  digitalWrite(HCT259lePin, HIGH);
  
  defaultHCT259();
  resetDAC(3);
  //gainDAC(3, true);  
}

uint8_t DACbuffer[16];
uint8_t tempBuffer[]={0,0};
uint8_t tempBuffer2[]={0,0};
uint16_t temp=0;

void gainDAC(uint8_t id, bool enable) {
  tempBuffer[0]=0b10000000;
  tempBuffer[1]=0b00110000;
  if (!enable) tempBuffer[1]=0;
  
  spiBridge.configureSPI(false, SPImode, SPIfrequency);
  switch (id) {
    case 3:
      changeHCT259pin(HCT259_SYNC1, LOW);
      spiBridge.spiTransfer( slaveNum, tempBuffer, sizeof(tempBuffer), tempBuffer2);
      defaultHCT259();
      changeHCT259pin(HCT259_SYNC2, LOW);
      spiBridge.spiTransfer( slaveNum, tempBuffer, sizeof(tempBuffer), tempBuffer2);
      defaultHCT259();
      break;
    case 2:
      changeHCT259pin(HCT259_SYNC2, LOW);
      spiBridge.spiTransfer( slaveNum, tempBuffer, sizeof(tempBuffer), tempBuffer2);
      defaultHCT259();    
      break;
    case 1:
      changeHCT259pin(HCT259_SYNC1, LOW);
      spiBridge.spiTransfer( slaveNum, tempBuffer, sizeof(tempBuffer), tempBuffer2);
      defaultHCT259();
      break;
  }
  
}
void resetDAC(uint8_t id) {
  tempBuffer[0]=0b11100000;
  tempBuffer[1]=0;
  
  spiBridge.configureSPI(false, SPImode, SPIfrequency);
  switch (id) {
    case 3:
      changeHCT259pin(HCT259_SYNC1, LOW);
      spiBridge.spiTransfer( slaveNum, tempBuffer, sizeof(tempBuffer), tempBuffer2);
      defaultHCT259();
      changeHCT259pin(HCT259_SYNC2, LOW);
      spiBridge.spiTransfer( slaveNum, tempBuffer, sizeof(tempBuffer), tempBuffer2);
      defaultHCT259();
      break;
    case 2:
      changeHCT259pin(HCT259_SYNC2, LOW);
      spiBridge.spiTransfer( slaveNum, tempBuffer, sizeof(tempBuffer), tempBuffer2);
      defaultHCT259();
      break;
    case 1:
      changeHCT259pin(HCT259_SYNC1, LOW);
      spiBridge.spiTransfer( slaveNum, tempBuffer, sizeof(tempBuffer), tempBuffer2);
      defaultHCT259();
      break;
  }
}
void writeDAC(uint8_t idDAC, uint8_t channel, uint8_t value) {


  DACbuffer[ (idDAC==2 ? 1 : 0) * 8 + (channel-1) ] = value;

  //Serial.println("DAC buf: " + String((idDAC==2 ? 1 : 0)));
  spiBridge.configureSPI(false, SPImode, SPIfrequency);
  //for (uint8_t i=0;i<=15;i++){    
    temp= value << 4;
    temp+= ( (channel-1) & 0b111) << 12 ;
    tempBuffer[0]=highByte(temp);
    tempBuffer[1]=lowByte(temp);
    //changeHCT259pin( ( i<=7 ? HCT259_SYNC1 : HCT259_SYNC2 ), LOW);
    //Serial.println("CS: " + String(( idDAC==2 ? HCT259_SYNC2 : HCT259_SYNC1 )));
    changeHCT259pin( ( idDAC==2 ? HCT259_SYNC2 : HCT259_SYNC1 ), LOW);
    spiBridge.spiTransfer( slaveNum, tempBuffer, sizeof(tempBuffer), tempBuffer2);
    defaultHCT259();

  //}
}
