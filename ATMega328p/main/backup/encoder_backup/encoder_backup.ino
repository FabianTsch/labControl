
#include <Encoder.h>
#include <Wire.h>

#define adresse 0x05


Encoder Enc(2, 3);
long oldPosition  = -999;
int sendSignal = 1;
int count = 0;
int data[4]={0};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Wire.begin(adresse);

  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);


  Serial.println("Bereit");
}



void loop() {
  long newPosition = Enc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }
}

void sendData(){
  
  if(count == 0){
    data[0] = oldPosition;
    data[1] = (oldPosition >> 8);
    data[2] = (oldPosition >> 16);
    data[3] = (oldPosition >> 24);
  }
  
  Wire.write(data[count]);
  Serial.println(data[count]);

  if(count < 3)
  count++;
  else
  count = 0;
}

void receiveData(int bytecount){
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
