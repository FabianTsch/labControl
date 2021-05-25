
#include <Encoder.h>
#include <Wire.h>
#include <HX711_ADC.h>
#include <EEPROM.h>

#define adresse 0x05

//Pins for HX711 :
const int HX711_dout_1 = 4; //mcu > HX711 no 1 dout pin
const int HX711_sck_1 = 5; //mcu > HX711 no 1 sck pin
const int HX711_dout_2 = 6; //mcu > HX711 no 2 dout pin
const int HX711_sck_2 = 7; //mcu > HX711 no 2 sck pin

//HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell_1(HX711_dout_1, HX711_sck_1); //HX711 1
HX711_ADC LoadCell_2(HX711_dout_2, HX711_sck_2); //HX711 2
float weight1 = 0;
float weight2 = 0;
byte* ptrWeight1 = (byte*)&weight1;
byte* ptrWeight2 = (byte*)&weight2;

const int calVal_eepromAdress_1 = 0; // eeprom adress for calibration value load cell 1 (4 bytes)
const int calVal_eepromAdress_2 = 4; // eeprom adress for calibration value load cell 2 (4 bytes)
unsigned long t = 0;

// Encoder
Encoder Enc(2, 3);
long oldPosition  = -999;
int sendSignal = 1;
int count = 0;
int data[12]={0};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  // Init HX711
  float calibrationValue_1; // calibration value load cell 1
  float calibrationValue_2; // calibration value load cell 2
  calibrationValue_1 = 502.17; // uncomment this if you want to set this value in the sketch // old value 281.19
  calibrationValue_2 = 568.08; // uncomment this if you want to set this value in the sketch // old value 275.55
#if defined(ESP8266) || defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch the value from eeprom
#endif
  //EEPROM.get(calVal_eepromAdress_1, calibrationValue_1); // uncomment this if you want to fetch the value from eeprom
  //EEPROM.get(calVal_eepromAdress_2, calibrationValue_2); // uncomment this if you want to fetch the value from eeprom
  
  LoadCell_1.begin();
  LoadCell_2.begin();
  unsigned long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  byte loadcell_1_rdy = 0;
  byte loadcell_2_rdy = 0;
  
  while ((loadcell_1_rdy + loadcell_2_rdy) < 2) { //run startup, stabilization and tare, both modules simultaniously
    if (!loadcell_1_rdy) loadcell_1_rdy = LoadCell_1.startMultiple(stabilizingtime, _tare);
    if (!loadcell_2_rdy) loadcell_2_rdy = LoadCell_2.startMultiple(stabilizingtime, _tare);
  }
  if (LoadCell_1.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 no.1 wiring and pin designations");
  }
  if (LoadCell_2.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 no.2 wiring and pin designations");
  }
  LoadCell_1.setCalFactor(calibrationValue_1); // user set calibration value (float)
  LoadCell_2.setCalFactor(calibrationValue_2); // user set calibration value (float)
  Serial.println("Startup is complete");
  Wire.begin(adresse);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  Serial.println("Bereit");
}



void loop() {
  // Encoder
  long newPosition = Enc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }

  // HX711
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity
  // check for new data/start next conversion:
  if (LoadCell_1.update()) newDataReady = true;
  LoadCell_2.update();

  //get smoothed value from data set
  if ((newDataReady)) {
    if (millis() > t + serialPrintInterval) {
      float a = LoadCell_1.getData();
      float b = LoadCell_2.getData();
      weight1 = a;
      weight2 = b;
      Serial.print("Load_cell 1 output val: ");
      Serial.print(a);
      Serial.print("    Load_cell 2 output val: ");
      Serial.println(b);
      newDataReady = 0;
      t = millis();
    }
  }
  
  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') {
      LoadCell_1.tareNoDelay();
      LoadCell_2.tareNoDelay();
    }
  }

  //check if last tare operation is complete
  if (LoadCell_1.getTareStatus() == true) {
    Serial.println("Tare load cell 1 complete");
  }
  if (LoadCell_2.getTareStatus() == true) {
    Serial.println("Tare load cell 2 complete");
  }
  
}


// Funktions
void sendData(){

  if(count == 0){
    data[0] = oldPosition;
    data[1] = (oldPosition >> 8);
    data[2] = (oldPosition >> 16);
    data[3] = (oldPosition >> 24);
    data[4] = ptrWeight1[0];
    data[5] = ptrWeight1[1];
    data[6] = ptrWeight1[2];
    data[7] = ptrWeight1[3];
    data[8] = ptrWeight2[0];
    data[9] = ptrWeight2[1];
    data[10] = ptrWeight2[2];
    data[11] = ptrWeight2[3];
  }
  
  Wire.write(data[count]);
  Serial.println(data[count]);

  if(count < 11)
  count++;
  else
  count = 0;
}

void receiveData(int bytecount){
  Serial.println("Daten erhalten");
  Enc.write(0);
  Serial.println(oldPosition);
  for(int i = 0; i < bytecount; i++){
    sendSignal = Wire.read();
  }
  if(sendSignal){
    digitalWrite(LED_BUILTIN, LOW);
    oldPosition = 0;
  }
  else{
    digitalWrite(LED_BUILTIN, HIGH); 
  }
}
