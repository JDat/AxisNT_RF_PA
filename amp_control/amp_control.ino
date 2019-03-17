#include "SC18IS602B.h"   // https://github.com/maxgerhardt/SC18IS602B-arduino

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

SC18IS602B spiBridge(-1,1,0,0);

#define WREN  6
#define WRDI  4
#define RDSR  5
#define WRSR  1
#define READ  3
#define WRITE 2

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
  
  spiBridge.begin();
  for ( uint8_t i=0; i<=2; i++){
    spiBridge.enableGPIO(HCT259addr[i], true);
    spiBridge.setupGPIO(HCT259addr[i], SC18IS601B_GPIO_MODE_PUSH_PULL);    
  }

  pinMode(HCT259dPin, OUTPUT);
  pinMode(HCT259lePin, OUTPUT);
  digitalWrite(HCT259dPin, HIGH);
  digitalWrite(HCT259lePin, HIGH);
  
  defaultHCT259();

  const int slaveNum = 0;

  spiBridge.configureSPI(false, SC18IS601B_SPIMODE_0, SC18IS601B_SPICLK_1843_kHz);

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

void loop() {
  static uint8_t prevState;
  //changeHCT259pin( HCT259_WE, prevState);
  prevState= prevState == HIGH ? LOW : HIGH;
  delay(5000);
}


