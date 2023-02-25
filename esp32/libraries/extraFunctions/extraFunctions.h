#ifndef EXTRAFUNCTIONS_H
#define EXTRAFUNCTIONS_H

#include "Adafruit_MCP4725.h"
#include "ADS1256.h"
#include "SparkFun_External_EEPROM.h"
#include <Wire.h>
#include "DHT.h"
#include "RTClib.h"
#include "WiFi.h"
#include "HTTPSRedirect.h"


#define latchPin 27 // connected to ST_CP
#define clockPin 25 // connected to SH_CP
#define dataPin 26  // connected to DS

#define button 99
#define LDR 36  // luminosity sensor
#define DHTPIN 4     // data pin
#define DHTTYPE DHT22 // type of DHT (DHT22)

// #define DEBUG true
//  --- auxiliar vars
extern int rangeCounter; // Contains current scale
extern int rel[7];       // HEX for each shunt resistance
extern int totalPoints;  // total points of reading
extern int pontosTotais;
extern float shunt[7];            // contains resistance values
extern int measurementsByTension; // number of measurements for the mean
extern float sumOfCurrents;       // variable to store the sum of currents for the mean
extern float humidity;
extern float temperature;
extern int luminosity;
extern int rangeCounter;
extern int multiplexer[7][3];

// URL to access the google sheet
// extern const char *GScriptId;
// extern String payload_base;
// extern String payload;
// extern const char *host;
// extern const int httpsPort;
// extern String url;

// array that store the received values of current and voltage
// extern String toSendData[40];

// --- objects ---
extern Adafruit_MCP4725 dac;
extern ADS1256 adc;
extern ExternalEEPROM eep;
extern DHT dht;
extern HTTPSRedirect* client;
extern RTC_DS1307 rtc;

// --- Additional functions ---

void setScale();  // sets input value for current scale       //ja foi
void scaleUp();   // sets up current scale                    //ja foi
void scaleDown(); // sets down current scale                  //ja foi

void Scan(int &var);        // similar to scanf                        //ja foi
void exportData(int &stop); // handles data exporting                  //ja foi

void sweep(); // voltage sweep function

void calibrating();
void cal2();

void voltageAdjust();

void savedata(int addres, float volt, float current, float dif);    // save data in the EEPROM
void readdata(int addres, float &volt, float &current, float &dif); // reads data from the EEPROM
float getDif(int addres);

void sendToSheet();                     // send the last measure to google sheet
void autonomousSweep();                  // make the measure from 0 to 2500 volts
void autonomous();                      // the equipament works autonomous
void meanOfMeasures(int numOfMeasures); // makes the measure and the mean
void scaleControl();           // checks the scale
void sweepControl(int startVoltage, int finalVoltage, int timestep); // loop that controls the measurement
void sensorsMeasure();      // read the dht and ldr
void connectToInternet();
void tempDataCopy();
void deleteCurrentData();
void saveDataToDrive();
#endif