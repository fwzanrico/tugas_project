// library untuk TFT LCD =  adafruit gfx
// with ST7789_Fast library link github : https://github.com/cbm80amiga/Arduino_ST7789_Fast.git
// link tutorial : https://www.youtube.com/watch?v=HoZhgNcJjNA


#include "LowPower.h"
#include "Arduino.h"
#define sensor 34
#define TFT_DC    7
#define TFT_RST   8 
#define SCR_WD   240
#define SCR_HT   240   // 320 - to allow access to full 240x320 frame buffer
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Arduino_ST7789_Fast.h>
Arduino_ST7789 lcd = Arduino_ST7789(TFT_DC, TFT_RST);


#include "gambarSenyum.h"
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

  lcd.init(SCR_WD, SCR_HT);
  lcd.fillScreen(BLACK);

  int i,j;
  for(j=0;j<7;j++) 
    for(i=0;i<7;i++)
      lcd.drawImageF(i*34,j*34,32,32,gambarSenyum);
  delay(4000);

  for(i=0;i<25;i++) {
    colorBar[i]    = RGBto565(i*256/25,0,i*256/25);
    colorBar[i+25] = RGBto565((24-i)*255/25,0,(24-i)*255/25);
  }
  for(i=0;i<240;i++) {
    lcd.drawImage(i,0,1,50,colorBar);
    lcd.drawImage(i,240-50,1,50,colorBar);
  }
  for(i=50;i<240-50;i++) {
    lcd.drawImage(0,i,50,1,colorBar);
    lcd.drawImage(240-50,i,50,1,colorBar);
  }
  delay(4000);
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
      lcd.drawImageF(random(0,240-32),random(0,240-32),32,32,gambarSenyum);
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
