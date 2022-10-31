#ifndef EXTRAFUNCTIONS_H
#define EXTRAFUNCTIONS_H

#include "Arduino.h"
#include "Adafruit_MCP4725.h"
#include "ADS1256.h"
#include "SparkFun_External_EEPROM.h"
#include <SoftwareSerial.h>

#define latchPin 8       // connected to ST_CP
#define clockPin 7       // connected to SH_CP
#define dataPin  6       // connected to DS

#define button 5         // external button, general purpose

//#define DEBUG true
// --- auxiliar vars
    extern int rangeCounter; // Contains current scale
    extern int rel[7];       // HEX for each shunt resistance
    extern int totalPoints;  // total points of reading 
    extern float shunt[7];   // contains resistance values

// --- utility vars ---
    extern float finalCurrent[295];     // saves current data
    extern float finalVoltage[295];     // saves voltage data


    
// --- objects --- 
        extern Adafruit_MCP4725 dac;
        extern ADS1256 adc;
        extern int rangeCounter;
        extern float shunt[7];
        extern ExternalEEPROM eep;
        extern int num_med;
        extern float measures;
        extern SoftwareSerial esp8266;



      // --- Additional functions ---  

        void setScale();         // sets input value for current scale       //ja foi
        void scaleUp();                 // sets up current scale                    //ja foi
        void scaleDown();               // sets down current scale                  //ja foi

        void Scan(int &var);            // similar to scanf                        //ja foi
        void exportData(int &stop);    // handles data exporting                  //ja foi

        void sweep();                   // voltage sweep function 

        void calibrating();
        void cal2();
        
        void voltageAdjust();

        void savedata(int addres,float volt, float current,float dif);
        void readdata(int addres,float &volt, float &current,float &dif);
        float getDif(int addres);
        void sendToESP();
#endif