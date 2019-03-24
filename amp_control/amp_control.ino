#include "SC18IS602B.h"   // https://github.com/maxgerhardt/SC18IS602B-arduino
#include "Temperature_LM75_Derived.h"   // https://github.com/jeremycole/Temperature_LM75_Derived
#include "AD7994.h"

#define logicSupply() adc1.readADC(3)
#define PA1Supply()   adc1.readADC(4)
#define PA2Supply()   adc2.readADC(4)

#define PA1fwd()      adc1.readADC(1)
#define PA1ref()      adc1.readADC(2)

#define PA2fwd()      adc2.readADC(1)
#define PA2ref()      adc2.readADC(2)


#define HCT259_EEPROM 1
#define HCT259_SYNC1  2
#define HCT259_SYNC2  3
#define HCT259_WE     7

#define HCT259_S0     0
#define HCT259_S1     1
#define HCT259_S2     2

#define HCT259dPin    A1
#define HCT259lePin   A2
#define ADCconvPin    A3

const uint8_t HCT259addr[]={ HCT259_S0, HCT259_S1, HCT259_S2};

AD7994  adc1(0, ADCconvPin);
AD7994  adc2(1, ADCconvPin);

Generic_LM75 tSens1(0x4A);

SC18IS602B spiBridge(-1,1,0,0);

#define WREN  6
#define WRDI  4
#define RDSR  5
#define WRSR  1
#define READ  3
#define WRITE 2

uint8_t slaveNum = 3;
uint8_t SPImode = SC18IS601B_SPIMODE_3;
uint8_t SPIfrequency = SC18IS601B_SPICLK_1843_kHz;
    
void changeHCT259pin( uint8_t pin, uint8_t state){
    for ( uint8_t i=0; i<=2; i++ ){
      spiBridge.writeGPIO(HCT259addr[i], (pin & (1<< i) ? HIGH : LOW ) );
    }
    digitalWrite(HCT259dPin, state);
    delay(1);
    digitalWrite(HCT259lePin, LOW);
    delay(1);
    digitalWrite(HCT259lePin, HIGH);
    delay(1);
}

void defaultHCT259(){
  for ( uint8_t i=0; i<=6; i++ ){
    changeHCT259pin( i, HIGH);
  }
  changeHCT259pin( HCT259_WE, LOW);
}

void setup() {
  Serial.begin(115200);

  adc1.begin();
  adc2.begin();

  adc1.config( 0b11111000 );
  adc2.config( 0b11111000 );

  //adc1.cycleTimer(0b00000111);
  //adc2.cycleTimer(0b00000111);
  adc1.cycleTimer(0);
  adc2.cycleTimer(0);  
  
  
  spiBridge.begin();
  for ( uint8_t i=0; i<=2; i++){
    spiBridge.enableGPIO(HCT259addr[i], true);
    spiBridge.setupGPIO(HCT259addr[i], SC18IS601B_GPIO_MODE_PUSH_PULL);
  }

//  spiBridge.enableGPIO(3, true);
//  spiBridge.setupGPIO(3, SC18IS601B_GPIO_MODE_PUSH_PULL);
//  spiBridge.writeGPIO(3, HIGH);
  
  pinMode(HCT259dPin, OUTPUT);
  pinMode(HCT259lePin, OUTPUT);
  digitalWrite(HCT259dPin, LOW);
  digitalWrite(HCT259lePin, HIGH);
  
  defaultHCT259();

  //readEEPROM();
  //EEPROMstatus(0b1111);
//  Serial.println("Status:\t" + String(EEPROMstatus(-1), HEX) );

  //int address=0x20;
//  for (int address=0;address<512;address++){
    //Serial.println();
    //Serial.println("Addr:\t" + String(address, HEX) + "\t\tData:\t" + String(readEEPROMByte(address), HEX) );
//  }  
}

String response="";

void loop() {
  //static uint8_t prevState;
  //changeHCT259pin( HCT259_WE, prevState);
  //Serial.println("Temperature (C):\t" + String(tSens.readTemperatureC()) );
  //prevState = prevState == HIGH ? LOW : HIGH;
  //delay(5000);
  doKurwa();
  
  adc1.doSample();
  adc2.doSample();

  //Serial.println(tSens1.readTemperatureC());
  response="ADCS,";
  response+=String(logicSupply())+",";
  
  response+=String(PA1Supply())+",";
  response+=String(PA2Supply())+",";

  response+=String(PA2fwd())+",";
  response+=String(PA1fwd())+",";

  response+=String(PA2fwd())+",";
  response+=String(PA1fwd())+",";

  response+=("*" + String(checkSum(response), HEX) );
  response="$" + response;
  response.toUpperCase();
  
  Serial.println(response);
}

char checkSum(String theseChars) {
  char check = 0;
  // iterate over the string, XOR each byte with the total sum:
  for (int c = 0; c < theseChars.length(); c++) {
    check = char(check ^ theseChars.charAt(c));
  } 
  // return the result
  return check;
}
void doKurwa() {
  bool kurwa=false;
  while (Serial.available() ) {
    Serial.read();
    kurwa=true;
  }

  if (kurwa) {
    Serial.println("$Kurwa!*7B");
  }  
}
/*
 * Read one byte from EEPROM
 * Address must be in range 0-511
 */
int readEEPROMByte(int address){
  uint8_t tmp;

  spiBridge.configureSPI(false, SPImode, SPIfrequency);

  tmp= READ | (address & 1 <<8) ? 1 << 3 : 0 << 3;
  changeHCT259pin( HCT259_EEPROM, LOW);
  
  if ( !spiBridge.spiTransfer( slaveNum, tmp ) )
      return -1;
  if ( !spiBridge.spiTransfer( slaveNum, byte(address) ) )
      return -1;
  
  tmp = spiBridge.spiTransfer( slaveNum, byte(address) );
  
  defaultHCT259();
  return tmp;
  
}

/*
 *  Read or write SPI EEPROM status.
 *  To read send in data parameter -1
 *  To write send in data value 0-255 according to EEPROM datashet.
 */
int EEPROMstatus(int data) {
  uint8_t cmd;

  uint8_t readBuf[]={0,0,0,0};

  
  spiBridge.configureSPI(false, SPImode, SPIfrequency);
  
  cmd = ( data<0 ) ? RDSR : WRSR;
  readBuf[0]=cmd;
  readBuf[1]=data;
  Serial.println("IN:");
  Serial.println("P: 0\t0x" + String(byte(readBuf[0]), HEX));
  Serial.println("P: 1\t0x" + String(byte(readBuf[1]), HEX));
  Serial.println("P: 2\t0x" + String(byte(readBuf[2]), HEX));
  Serial.println("P: 3\t0x" + String(byte(readBuf[3]), HEX));


  changeHCT259pin( HCT259_EEPROM, LOW);
  spiBridge.writeGPIO(3, HIGH);
  if ( spiBridge.spiTransfer( slaveNum, readBuf, 4, readBuf) == false ) {
      Serial.println("CMD tx");
      return -1;
  }
  Serial.println("OUT:");
  Serial.println("P: 0\t0x" + String(byte(readBuf[0]), HEX));
  Serial.println("P: 1\t0x" + String(byte(readBuf[1]), HEX));
  Serial.println("P: 2\t0x" + String(byte(readBuf[2]), HEX));
  Serial.println("P: 3\t0x" + String(byte(readBuf[3]), HEX));
  //cmd = spiBridge.spiTransfer( slaveNum, cmd );
  //cmd = spiBridge.spiTransfer( slaveNum, cmd );
  //cmd = spiBridge.spiTransfer( slaveNum, cmd );
  //cmd = spiBridge.spiTransfer( slaveNum, cmd );
  spiBridge.writeGPIO(3, LOW);
  defaultHCT259();
  return cmd;
}
void readEEPROM(){

  spiBridge.configureSPI(false, SPImode, SPIfrequency);

  uint8_t rxBuffer[130];
  uint8_t txBuffer[2];
  bool ok=false;
  
  //for (uint8_t j=0; j<=0x1; j++) {
    txBuffer[0]=READ;
    txBuffer[1]=0;

    changeHCT259pin( HCT259_EEPROM, LOW);
    ok = spiBridge.spiTransfer(slaveNum, txBuffer, sizeof(txBuffer), rxBuffer, sizeof(rxBuffer) );
    defaultHCT259();
    
    if(ok) {  
      for(size_t i = 0; i < sizeof(rxBuffer); i++) {
        Serial.println("rxBuffer[0x" + String(i, HEX) + "] = 0x" + String(rxBuffer[i], HEX)+";");
      }
      Serial.println();
    } else {
      Serial.println("SPI transfer failed");
    }
  //}  
}

