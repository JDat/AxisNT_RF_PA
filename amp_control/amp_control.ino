#include "Temperature_LM75_Derived.h"   // https://github.com/jeremycole/Temperature_LM75_Derived
#include "AD7994.h"
#include "AD5308_DAC.h"

/*
 * Serial commands
 * from arduino:
 * $ADC status
 * $DAC status
 * $EEPROM contents
 * $KURWA
 * From PC:
 * $Get EEPROM contents
 * $Store DAC values to EEPROM
 * $Configure DAC
 * $Set DAC values
 * $PTT - push to talk
 */
 
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


#define ADCconvPin    A3

#define PTT1Pin       12
#define PTT2Pin       11

AD7994  adc1(0, ADCconvPin);
AD7994  adc2(1, ADCconvPin);

Generic_LM75 tSens1(0x4A);

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
  
  pinMode(PTT1Pin, OUTPUT);
  pinMode(PTT2Pin, OUTPUT);
  digitalWrite(PTT1Pin, HIGH);
  digitalWrite(PTT2Pin, HIGH);

  void setupDAC();
  
}

String response="";

void loop() {

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
