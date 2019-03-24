/*
 * AD7994.h
 *
 * Library for interfacing with the Analog Devices AD7994 I2C Analog to Digital Converter
 */

#ifndef SRC_AD7994_H_
#define SRC_AD7994_H_

#include <stddef.h>
#include <stdint.h>

#define     AD7994base     0x21

#define     AD7994externalSampleDelay       1

#define     AD7994_REG_CONVERSION_RESULT    0x0
#define     AD7994_REG_ALERT_STATUS         0x1
#define     AD7994_REG_CONFIG               0x2
#define     AD7994_REG_CYCLE_TIMER          0x3
#define     AD7994_REG_CH1_DATA_LOW         0x4
#define     AD7994_REG_CH1_DATA_HIGH        0x5
#define     AD7994_REG_CH1_HYSTERESIS       0x6
#define     AD7994_REG_CH2_DATA_LOW         0x7
#define     AD7994_REG_CH2_DATA_HIGH        0x8
#define     AD7994_REG_CH2_HYSTERESIS       0x9
#define     AD7994_REG_CH3_DATA_LOW         0xA
#define     AD7994_REG_CH3_DATA_HIGH        0xB
#define     AD7994_REG_CH3_HYSTERESIS       0xC
#define     AD7994_REG_CH4_DATA_LOW         0xD
#define     AD7994_REG_CH4_DATA_HIGH        0xE
#define     AD7994_REG_CH4_HYSTERESIS       0xF

class AD7994 {
public:
    /* By default instantiate it with PIN A to GND */
    //AD7994() : AD7994(0, -1) {};
    //AD7994(uint8_t addr) : AD7994(addr, -1) {};

    AD7994(uint8_t addr=0, int convPin=-1);
    
    virtual ~AD7994();
    
    /* calls into Wire.begin() */
    void begin();
    bool config(uint8_t conf);
    uint8_t config();
    bool alertStatus(uint8_t conf);
    uint8_t alertStatus();
    bool cycleTimer(uint8_t conf);
    uint8_t cycleTimer();
    bool hysteresis(uint8_t channel, uint8_t value);
    uint8_t hysteresis(uint8_t channel);
    bool limit(uint8_t channel, uint16_t value);
    uint16_t limit(uint8_t channel);
    uint16_t readADC(uint8_t channel);
    void doSample();
    
private:

    uint8_t address = AD7994base;        /* address of the module */
    int thePin;

    uint16_t convResult[4];
    
    const uint8_t hysteresisArray[4]={
    AD7994_REG_CH1_HYSTERESIS,
    AD7994_REG_CH2_HYSTERESIS,
    AD7994_REG_CH3_HYSTERESIS,
    AD7994_REG_CH4_HYSTERESIS};

    const uint8_t dataLimitArray[4]={
    AD7994_REG_CH1_DATA_LOW,
    AD7994_REG_CH2_DATA_LOW,
    AD7994_REG_CH3_DATA_LOW,
    AD7994_REG_CH4_DATA_LOW};

    //uint16_t convData[4];
    void ADCdoConv();
    bool i2c_writeByte(uint8_t reg, uint8_t data );
    uint8_t i2c_readByte(uint8_t reg );
    bool i2c_writeWord(uint8_t reg, uint16_t data );
    uint16_t i2c_readWord(uint8_t reg );
    void i2c_readConversation();
};

#endif /* SRC_AD7994_H_ */
