#include <Wire.h>
#include "ADS1256.h"
#include "WiFi.h"
#include "extraFunctions.h"
#include "Adafruit_MCP4725.h"
#include "SparkFun_External_EEPROM.h"
#include "DHT.h"
#include "HTTPSRedirect.h"
#include <NTPClient.h>

const char* GScriptId   = "AKfycbyn4mBgOtGKilT11P1RbFAfTR9Eo_A_rrZrnz2an1NAiAQg7wc4zd_gm23lv09iTxxU";
String payload_base =  "{\"command\": \"append_row\", \"sheet_name\": \"Sheet1\", \"values\": ";
String payload = "";
const char* host = "script.google.com";
const int httpsPort = 443;
String url = String("/macros/s/") + GScriptId + "/exec?cal";

//array that store the received values of current and voltage
String toSendData[40];


// --- pins definition ---
#define latchPin 27       // connected to ST_CP
#define clockPin 25       // connected to SH_CP
#define dataPin  26       // connected to DS

int measurementsByTension = 1;
float sumOfCurrents = 0;

float humidity;
float temperature;
int luminosity;

// --- objects ---
Adafruit_MCP4725 dac;
ADS1256 adc;
ExternalEEPROM eep;
DHT dht(DHTPIN, DHTTYPE);
WiFiUDP udp;
HTTPSRedirect* client = nullptr;
NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 60000);


// --- utility values ---
int rel[7] = {0x00,           // HEX for each shunt resistance
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

int multiplexer[8][3] = {{0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1}, {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1}};


// --- auxiliar vars ---
int rangeCounter = 0;        // current scale starts at lowest resistance

int totalPoints = 0;         // total points of reading

void CallISR();// interruption routine, detects falling edge

void setup() {
  delay(100);
  Serial.begin(115200);
  dht.begin();
  ntp.begin();
  ntp.forceUpdate();
  pinMode(LDR, INPUT);
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

  if (eep.begin(0b1010000, Wire) == false) {
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
}


void loop() {
  if (Serial.available() > 0) {
    int input = 0;
    Scan(input);
    Serial.println(input);
    delay(1000);

    if (input == 1) { // starts manual calibration
      sweep();
    }// end input verification

    else if (input == 2) {
      calibrating();
    }
    else if (input == 3) {
      cal2();
    }
    else if (input == 4) {
      voltageAdjust();
    }
    else if (input == 5) {
      Serial.println("digite valor de leituras:");
      Scan(input);
      exportData(input);
    }
    else if (input == 6) {
      sendToSheet();
    }
    else if (input == 7) {
      autonomous();
    }
  }// end serial scan
}

void CallISR() {

  adc.DRDY_Interuppt();

}
