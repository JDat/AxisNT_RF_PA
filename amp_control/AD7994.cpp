#include "AD7994.h"
#include <Arduino.h>
#include <Wire.h>

AD7994::AD7994(uint8_t addr=0, int convPin)
    {
      if (addr) address++;
      thePin=convPin;
}

AD7994::~AD7994() {
}
void AD7994::begin() {
  if (thePin>=0) {
    pinMode(thePin, OUTPUT);
    digitalWrite(thePin, LOW);
  }
    Wire.begin();
}
bool AD7994::config(uint8_t conf) {
  return this->i2c_writeByte(AD7994_REG_CONFIG, conf);
}
uint8_t AD7994::config() {
  return this->i2c_readByte(AD7994_REG_CONFIG);
}
bool AD7994::alertStatus(uint8_t conf) {
  return this->i2c_writeByte(AD7994_REG_ALERT_STATUS, conf);
}
uint8_t AD7994::alertStatus() {
  return this->i2c_readByte(AD7994_REG_ALERT_STATUS);
}
bool AD7994::cycleTimer(uint8_t conf) {
  return this->i2c_writeByte(AD7994_REG_CYCLE_TIMER, conf);
}
uint8_t AD7994::cycleTimer() {
  return this->i2c_readByte(AD7994_REG_CYCLE_TIMER);
}
bool AD7994::hysteresis(uint8_t channel, uint8_t value) {
  return this->i2c_writeByte(hysteresisArray[channel-1], value);
}
uint8_t AD7994::hysteresis(uint8_t channel){
  return this->i2c_readByte(hysteresisArray[channel-1]);
}
bool AD7994::limit(uint8_t channel, uint16_t value){
  return this->i2c_writeWord(dataLimitArray[channel-1], value);
}
uint16_t AD7994::limit(uint8_t channel){
  return this->i2c_readWord(dataLimitArray[channel-1]);
}
bool AD7994::i2c_writeByte(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(data);
  return Wire.endTransmission() == 0;
}
uint8_t AD7994::i2c_readByte(uint8_t reg ) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();
  while (Wire.requestFrom(address,1) == 0 );
  while (Wire.available() == 0 );
  return Wire.read();  
}
bool AD7994::i2c_writeWord(uint8_t reg, uint16_t data ){
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(highByte(data));
  Wire.write(lowByte(data));
  return Wire.endTransmission() == 0;
}
uint16_t AD7994::i2c_readWord(uint8_t reg ){
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();
  while (Wire.requestFrom(address,2) == 0 );
  while (Wire.available() == 0 );
  uint8_t a=Wire.read();
  uint8_t b=Wire.read();
  uint16_t temp=(a<<8) || b;
  return temp;
}
void AD7994::ADCdoConv() {
  if (thePin >=0) {
    digitalWrite(thePin, HIGH);
    delay(AD7994externalSampleDelay);
    digitalWrite(thePin, LOW);
    delay(AD7994externalSampleDelay);    
  }
}

uint16_t AD7994::readADC(uint8_t channel){
  return convResult[channel-1];
}
void AD7994::i2c_readConversation(){
  for (uint8_t i=0;i<=3;i++){
    ADCdoConv();
    Wire.beginTransmission(address);
    Wire.write(AD7994_REG_CONVERSION_RESULT);
    //Wire.write(0b11110000);
    Wire.endTransmission();
    Wire.requestFrom(address,2);
    //uint8_t j=0;
    while (Wire.available()) {
      uint8_t a=Wire.read();
      uint8_t b=Wire.read();
      uint16_t temp=(a<<8) | b;
      uint8_t c= (a & ( 0b11 << 4 )) >> 4 ;
      //Serial.println("Readed. c: " + String(c) + " a: " + String(a, BIN) +" b: " + String(b, BIN) + " temp: " + String(temp, BIN) + " i: " + String(i));
      convResult[c]=temp & 0xfff;
      //j++;
    }
  }
}

void AD7994::doSample(){
  i2c_readConversation();
}

