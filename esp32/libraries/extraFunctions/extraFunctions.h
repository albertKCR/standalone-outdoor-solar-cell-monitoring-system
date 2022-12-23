#ifndef EXTRAFUNCTIONS_H
#define EXTRAFUNCTIONS_H

#include "Arduino.h"
#include "Adafruit_MCP4725.h"
#include "ADS1256.h"
#include "SparkFun_External_EEPROM.h"
#include <Wire.h>
#include "DHT.h"
#include <String.h>


#define latchPin 27       // connected to ST_CP
#define clockPin 25       // connected to SH_CP
#define dataPin  26       // connected to DS

#define button 5         // external button, general purpose

#define LDR 36 //luminosity sensor
#define DHTPIN 34 // data pin
#define DHTTYPE DHT11 // type of DHT (DHT11)

//#define DEBUG true
// --- auxiliar vars
    extern int rangeCounter; // Contains current scale
    extern int rel[7];       // HEX for each shunt resistance
    extern int totalPoints;  // total points of reading 
    extern int pontosTotais;
    extern float shunt[7];   // contains resistance values
    extern int measurementsByTension; // number of measurements for the mean 
    extern float sumOfCurrents; // variable to store the sum of currents for the mean
    extern float humidity;
    extern float temperature;
    extern int luminosity;
    extern int rangeCounter;

// --- utility vars ---
    extern float finalCurrent[295];     // saves current data
    extern float finalVoltage[295];     // saves voltage data


    
// --- objects --- 
        extern Adafruit_MCP4725 dac;
        extern ADS1256 adc;
        extern ExternalEEPROM eep;
        extern DHT dht;
        extern HTTPSRedirect* client = nullptr;


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

        void savedata(int addres,float volt, float current,float dif); //save data in the EEPROM
        void readdata(int addres,float &volt, float &current,float &dif); //reads data from the EEPROM
        float getDif(int addres);

        void sendToSheet(); //send the last measure to google sheet
        void autonomousSweep(); //make the measure from 0 to 2500 volts
        void autonomous();  //the equipament works autonomous
        void meanOfMeasures(int numOfMeasures); //makes the measure and the mean
        void scaleControl(int range); //checks the scale
        void sweepScaleControl(int range, int *i); //checks the scale changing when 'sweep' is being executed
        void sweepControl(int startVoltage, int finalVoltage, int rangeCounter, int measurementsByTension, int timestep);  //loop that controls the measurement
        void sensorsMeasure();
#endif