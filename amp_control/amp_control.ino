#include "SC18IS602B.h"   // https://github.com/maxgerhardt/SC18IS602B-arduino

#define HCT259_EEPROM 1
#define HCT259_SYNC1  2
#define HCT259_SYNC2  3
#define HCT259_WE     7

#define HCT259_S0     0
#define HCT259_S1     1
#define HCT259_S2     2

#define HCT259dPin    4
#define HCT259lePin   3

const uint8_t HCT259addr[]={ HCT259_S0, HCT259_S1, HCT259_S2};

SC18IS602B spiBridge(-1,1,0,0);

#define WREN  6
#define WRDI  4
#define RDSR  5
#define WRSR  1
#define READ  3
#define WRITE 2

char buf [130];

void fill_buffer()
{
  buf[0]=READ;
  buf[1]=0;
  //buf[2]=0;
  for (int I=2;I<sizeof(buf);I++)
  {
    buf[I]=0xaa;
  }
}


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

  const int slaveNum = 3;
  const uint8_t cmd = 0x3;

  spiBridge.configureSPI(false, SC18IS601B_SPIMODE_0, SC18IS601B_SPICLK_1843_kHz);

  fill_buffer();
  
  for (uint8_t j=0; j<=0xf; j++) {
    //uint8_t spiData[] = { cmd, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa };
    //uint8_t spiData[] = { 0x05 };
    //uint8_t spiData[] = { 0x01, 0b10001100 };
    //uint8_t spiData[] = { READ, 0x0, 0x55, 0 };
    //spiData[1]= j << 4;
    uint8_t spiReadBuf[sizeof(buf)];
  
    //Serial.println("j=" + String(spiData[1], HEX) );


    //changeHCT259pin( HCT259_EEPROM, LOW);
    //changeHCT259pin( HCT259_WE, LOW);
    //bool ok = spiBridge.spiTransfer(slaveNum, byte(WREN));
    //defaultHCT259();
    
    //changeHCT259pin( HCT259_EEPROM, LOW);
    //changeHCT259pin( HCT259_WE, HIGH);
    //ok = spiBridge.spiTransfer(slaveNum, buf, sizeof(buf), spiReadBuf);
    //defaultHCT259();

    //if(ok) {  
    //  for(size_t i = 0; i < sizeof(buf); i++) {
    //    Serial.print("SPI wrBuf[" + String(i) + "] = 0x" + String(buf[i], HEX)+"\t");
    //    Serial.println("readBuf[" + String(i) + "] = 0x" + String(spiReadBuf[i], HEX));
    //  }
    //} else {
    //  Serial.println("SPI transfer failed");
    //} 

    //buf[0]=READ;
    changeHCT259pin( HCT259_EEPROM, LOW);
    bool ok = spiBridge.spiTransfer(slaveNum, buf, sizeof(buf), spiReadBuf);
    defaultHCT259();
    
    if(ok) {  
      for(size_t i = 0; i < sizeof(buf); i++) {
        Serial.print("SPI wrBuf[" + String(i) + "] = 0x" + String(buf[i], HEX)+"\t");
        Serial.println("SPI readBuf[" + String(i) + "] = 0x" + String(spiReadBuf[i], HEX));
      }
    } else {
      Serial.println("SPI transfer failed");
    }  
  }
  
}

void loop() {
  static uint8_t prevState;
  //changeHCT259pin( HCT259_WE, prevState);
  prevState= prevState == HIGH ? LOW : HIGH;
  delay(5000);
}


