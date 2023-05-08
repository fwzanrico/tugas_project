// library untuk TFT LCD =  adafruit gfx
// with ST7789_Fast library link github : https://github.com/cbm80amiga/Arduino_ST7789_Fast.git
// link tutorial : https://www.youtube.com/watch?v=HoZhgNcJjNA


#include "LowPower.h"
#include "Arduino.h"
#define sensor 34
int RELAY = 33;
const int interruptPin = 2;
int desiredValue = 200;
volatile int state = 0;
//buat tanda trigger interrupt
volatile int trigger = 0;

void setup() {
  Serial.begin(9600);
  pinMode(sensor, INPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
}
void wakeUp();
void adjust_moisture(){
  //asumsikan aja ini LED buat representasi pompa air kita
  digitalWrite(RELAY,HIGH);
  int value = analogRead(sensor);
  //Pompa bakal nyala selama hasil ukurnya belum sesuai yang diinginkan
  while(value < desiredValue){
      value = analogRead(sensor);
      Serial.print("New Value: ");
      Serial.println(value);    
  }
  // setelah sesuai langsung dimatikan
  digitalWrite(RELAY, LOW);
  // ilangin interrupt supaya nanti ppush button gak bisa dinpcet sampai masuk ke state 1 lagi
  
  //kembali ke kondisi belum ke trigger
  trigger = 0;
  //kembali ke state idle
  state = 0;
}
void loop() {
  int sensorValue = analogRead(sensor);
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);
  if(sensorValue < desiredValue){
    state = 1;
  } 

  switch(state){
    case 0:      
      break;
    case 1:
    //ku attach intterrupt di state 1 supaya nanti kalo misal belum ke state 1, dia push buttonnya gak ngaruh kalo ditekan
      attachInterrupt(digitalPinToInterrupt(interruptPin), wakeUp, FALLING);
    //kalo push button di tekan, panggi fungsi ini
      if(trigger == 1)adjust_moisture();
   		 detachInterrupt(0);
      break;
    default:
    break;
  };
   // LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
  
}
void wakeUp(){
 trigger = 1;  
}
