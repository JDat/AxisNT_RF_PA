#include "SC18IS602B.h"   // https://github.com/maxgerhardt/SC18IS602B-arduino
#include "Temperature_LM75_Derived.h"   // https://github.com/jeremycole/Temperature_LM75_Derived
#include "AD7994.h"

/*
 * ADC ports
 * ADC1
 * ch 1 - Amp1 forward power
 * ch 2 - Amp1 reflected power
 * ch 3 - Logic supply ( +6V )
 * ch 4 - Amp1 HV ( +28V ) supply
 * ADC2
 * ch 1 - Amp2 forward power
 * ch 2 - Amp2 reflected power
 * ch 3 - Unknown. Maybe logic supply ( +6V ) with big sensitivity
 * ch 4 - Amp2 HV ( +28V ) supply
 */
#define logicSupply() adc1.readADC(3)
#define PA1Supply()   adc1.readADC(4)
#define PA2Supply()   adc2.readADC(4)

#define PA1fwd()      adc1.readADC(1)
#define PA1ref()      adc1.readADC(2)

#define PA2fwd()      adc2.readADC(1)
#define PA2ref()      adc2.readADC(2)

// ADC coeficients
#define Vref              2.5f      // Measured on ADC pin
#define logicSupplyCoef   4.91f     // Measured on connector PL3, pin 7

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
#define ADCconvPin    A3

#define PTT1Pin       12
#define PTT2Pin       11
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

  pinMode(PTT1Pin, OUTPUT);
  pinMode(PTT2Pin, OUTPUT);
  digitalWrite(PTT1Pin, HIGH);
  digitalWrite(PTT2Pin, HIGH); 

  resetDAC(3);
  gainDAC(3, true);

}

String response="";

void loop() {
/*
  static uint8_t prevState;
  //changeHCT259pin( HCT259_WE, prevState);

  // AMP1
  //writeDAC(1,1,prevState);      //TR14 3rd stage AB class
  //writeDAC(1,2,prevState);      //TR15 3rd stage C class
  //writeDAC(1,3,prevState);      //TR12 2nd stage
  writeDAC(1,4,prevState);      //TR11 1st stage
  
  //writeDAC(1,5,prevState);
  //writeDAC(1,6,prevState);
  //writeDAC(1,7,prevState);      //TR14 3rd stage AB class
  //writeDAC(1,8,prevState);

  // AMP2
  //writeDAC(2,1,prevState);      //TR20 3rd stage AB class
  //writeDAC(2,2,prevState);      //TR25 3rd stage C class
  //writeDAC(2,3,prevState);      //TR28 2nd stage
  writeDAC(2,4,prevState);      //TR3 1st stage
  
  //writeDAC(2,5,prevState);
  //writeDAC(2,6,prevState);
  //writeDAC(2,7,prevState);      //TR20 3rd stage AB class
  //writeDAC(2,8,prevState);

//    for (uint8_t j=0; j<255;j++){
//      //unsigned long a=millis();
//      writeDAC(1,1,j);
//      writeDAC(1,7,j);
//      //Serial.print("Millis: ");
//      //Serial.println(millis()-a);
//      Serial.println( "j:\t" + String(j) );
//  }
  Serial.print("prevstate: ");
  Serial.println( prevState );
  prevState = prevState == 255 ? 0 : 255;
  delay(5000);
*/
  if (Serial.available()) doKurwa();
  
  adc1.doSample();
  adc2.doSample();

  sendTelemetry();
  delay(100);
}

void sendTelemetry(){
  const float logicVoltMult = Vref / 4096.0f * logicSupplyCoef;
  response="ADCS,";
  response+=String(logicSupply()*logicVoltMult)+"V,";
  
  response+=String(PA1Supply())+",";
  response+=String(PA2Supply())+",";

  //response+=String(tSens1.readTemperatureC())+"C,";
  response+=String( round(tSens1.readTemperatureC()) )+"C,";
  
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
  while (Serial.available() ) {
    Serial.read();
    Serial.println("$Kurwa!*7B");
  }
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

