/*
 * AD5308.h
 *
 * Library for interfacing with the Analog Devices AD5808 SPI Digital to Analog Converter
 * for Axis NT RF PA
 */
#ifndef SRC_AD5308_H_
#define SRC_AD5308_H_

/*
 * DAC ports
 * DAC1
 * ch1 - TR14 3rd stage AB class (higher voltage, have two channels)
 * ch2 - TR15 3rd stage C class  (lower voltage, have one channel)
 * ch3 - TR12 2nd stage
 * ch4 - TR11 1st stage
 * ch5 -
 * ch6 -
 * ch7 - TR14 3rd stage AB class (higher voltage, have two channels)
 * ch8 -
 * 
 * DAC2
 * ch1 - TR20 3rd stage AB class (higher voltage, have two channels)
 * ch2 - TR25 3rd stage C class  (lower voltage, have one channel)
 * ch3 - TR28 2nd stage
 * ch4 - TR3  1st stage
 * ch5 -
 * ch6 -
 * ch7 - TR20 3rd stage AB class (higher voltage, have two channels)
 * ch8 -
 */

#define HCT259_EEPROM 1
#define HCT259_SYNC1  2
#define HCT259_SYNC2  3
#define HCT259_WE     7

#define HCT259_S0     0
#define HCT259_S1     1
#define HCT259_S2     2

#define HCT259dPin    A1
#define HCT259lePin   A2


void changeHCT259pin( uint8_t pin, uint8_t state);
void defaultHCT259();
void setupDAC();
void gainDAC(uint8_t id, bool enable);
void resetDAC(uint8_t id);
void writeDAC(uint8_t idDAC, uint8_t channel, uint8_t value);
#endif /* SRC_AD5308_H_ */
