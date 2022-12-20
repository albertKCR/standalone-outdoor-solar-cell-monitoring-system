#include <RTClib.h>
#include <Wire.h>
#include <Arduino.h>
#include "ADS1256.h"
#include "extraFunctions.h"
#include "Adafruit_MCP4725.h"
#include <SoftwareSerial.h>
#include "SparkFun_External_EEPROM.h"

  int measurementsByTension=1;
  float sumOfCurrents=0;

// --- objects ---
  Adafruit_MCP4725 dac;    // DAC object
  ADS1256 adc;
  ExternalEEPROM eep;
  SoftwareSerial esp8266(3, 4);
  RTC_DS1307 rtc;

// --- pins definition ---
  #define latchPin 8       // connected to ST_CP  chip pin 12
  #define clockPin 7       // connected to SH_CP  chip pin 11
  #define dataPin  6       // connected to DS     chip pin 14

  #define button 5         // multisuse button


// --- utility values ---
  int rel[7]= {0x00,            // HEX for each shunt resistance
    0x08,
    0x02,
    0x12,
    0x01,
    0x21,
    0x05
  };


  float shunt[7] = { // Shunt resistance values
    0.0996086,     
    1.276498,   // limite 20.8mA  
    9.87943169,   // limite 2.5mA   * 1000
    96.8096892,   // limite 250uA   110,7894040841325
    973.291872,  // limite 25uA
    9678.0492, // limite 2.5uA   * 1 000 000
    97611.816 // limite 250nA   * 1 000 000 000
  };                
                                         

// --- auxiliar vars --- 
  int rangeCounter = 0;        // current scale starts at lowest resistance  

  int totalPoints = 0;         // total points of reading

// --- utility vars ---
   
void CallISR();// interruption routine, detects falling edge

void setup() {
  delay(100);
  Serial.begin(115200);
  esp8266.begin(115200);
  Serial.println("booting");

//--- 74HC595 pins as outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin , OUTPUT);

  pinMode(button, INPUT_PULLUP);                  // button as input

  digitalWrite(latchPin, HIGH);                   // LATCH pin starts ON
//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---

  scaleUp();
  delay(300);
  scaleDown();
  Wire.begin();

  if (eep.begin(0b1010000, Wire) == false){
    Serial.println("333");
    Serial.println("No memory detected. Freezing.");
    while (1);
  }

  Serial.println("memory ok");
  attachInterrupt(digitalPinToInterrupt(ADS_RDY_PIN), CallISR, FALLING); // calls routine whenever occurs falling edge


  dac.begin(0x60);
  dac.setVoltage(0x2AB, false);
    
//initialize the ADS
  adc.begin();
// --- RTC ---
  if (!rtc.begin()){
    Serial.println("DS1307 not found");
    while(1);
  }
  if (!rtc.isrunning()){
    Serial.println("DS1307 working");
    //time adjust
    rtc.adjust(DateTime(2022, 11, 23, 11, 59, 20));
  }
// ---

  Serial.println("done init");  
}


void loop() {
  if(Serial.available()>0){
    int input = 0;
    Scan(input);
    Serial.println(input);
    delay(1000);
        
    if(input == 1){ // starts manual calibration
      sweep();          
    }// end input verification
        
    else if(input == 2){
      calibrating(); 
    }
    else if(input == 3){
      cal2();
    }
    else if(input == 4){
      voltageAdjust();
    }
    else if(input == 5){
      Serial.println("digite valor de leituras:");
      Scan(input);
      exportData(input);
    }
    else if(input==6){
      sendToESP();
    }
    else if(input==7){
      autonomous();
    }
  }// end serial scan
}

void CallISR(){

  adc.DRDY_Interuppt();

}
