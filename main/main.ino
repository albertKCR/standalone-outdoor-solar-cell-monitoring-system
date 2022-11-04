#include <Arduino.h>
#include "ADS1256.h"
#include "extraFunctions.h"
#include "Adafruit_MCP4725.h"
#include <SoftwareSerial.h>
#include "SparkFun_External_EEPROM.h"

ExternalEEPROM eep;
SoftwareSerial esp8266(3, 4);

/* Keep this commented...
    THESE ARE THE PINS ON THE ARDUINO UNO YOU SHOULD WIRE THESE TO
    CLK  - pin 13
    DIN  - pin 11 (MOSI)
    DOUT - pin 12 (MISO)
*/  int num_med=1;
    float measures=0;

// --- Objects --- 
    Adafruit_MCP4725 dac;    // DAC object
    ADS1256 adc;
    int pontosTotais;

// ---  Pin names --- 
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
/*
    float shunt[7] = { 0.600,     // Shunt resistance values
                    1.200,
                    10.00,
                    98.30,
                    979.00,
                    9920.00,
                    99800.00
                    };                
       */

    /*      float shunt[7] = { // Shunt resistance values
                    0.2370370370370,     
                    1.3072289,   // limite 20.8mA  
                    10.7298001737619,   // limite 2.5mA   * 1000
                    97.966528,   // limite 250uA   110,7894040841325
                    989.5021920,  // limite 25uA
                    10447.956, // limite 2.5uA   * 1 000 000
                    100360.8 // limite 250nA   * 1 000 000 000
                    };  
*/


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
  esp8266.begin(115200);
  delay(100);
  Serial.begin(115200);
  Serial.println("booting");
//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---
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
  Serial.println("333");

 if (eep.begin(0b1010000, Wire) == false) //And Uno will fail to compile here
  {
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


   
  Serial.println("done init");



  //determine the conversion factor
  //do some calculations for the constants
  
}


void loop() {
    esp8266.println(0.988, 1);
    //Serial.println(totalPoints, 1);
    delay(4000);
    
    if(Serial.available()>0){
        int input = 0;
        //input = Serial.read();
        Scan(input);
        Serial.println(input);
        //input = input - 48;
        //Serial.println(input);
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

    }// end serial scan

}

void CallISR(){

  adc.DRDY_Interuppt();

}
