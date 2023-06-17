
#ifndef ADS1256_H
#define ADS1256_H

#include "ADS_DATA.h"
#include "Arduino.h"
#include <SPI.h>
#include "SparkFun_External_EEPROM.h"
extern int rangeCounter;
extern ExternalEEPROM eep;

class ADS1256
{

#define ADS_RST_PIN 17//15 // ADS1256 reset pin
#define ADS_RDY_PIN 16 // ADS1256 data ready
#define ADS_CS_PIN 5 // ADS1256 chip select


//#define ADS_RST_PIN 9 // ADS1256 reset pin
//#define ADS_RDY_PIN 2 // ADS1256 data ready
//#define ADS_CS_PIN 53 // ADS1256 chip select
     
     boolean DRDY_state;

public:
    float OutputVoltage;        // stores readed voltage
    float OutputCurrent;        // stores readed current

    bool top_overflow;          // overvoltage flag
    bool bottom_overflow;       // undervoltage flag

    float diffe =0.0;           // stores differential voltage applied to device

    float correction[7];



    double val1; // saves digital data coming from ADC // voltage data
    
    double val2; // saves digital data coming from ADC // current data

    // put the ADC constants here

    double resolution ;//= 8388608.; // 2^23-1

    // this needs to match the setting in the ADC init function in the library tab
    double Gain ;//= 1.; // be sure to have a period

    double vRef;// = 5.0458; // reference voltage

    // we'll calculate this in setup
    double bitToVolt; // = 0.;

    ADS1256();
    void begin();
    void read_two_values();
    void waitforDRDY();
    void DRDY_Interuppt();
    long GetRegisterValue(uint8_t regAdress);
    void Reset();
    void SetRegisterValue(uint8_t regAdress, uint8_t regValue);
    void SendCMD(uint8_t cmd);


    void average();
    void average(int value);
    void conversion(float res);

    bool topOV();       // flag do overflow superior
    bool botOV();       // flag do overflow inferior

    float returnVoltage();
    float returnCurrent();

    unsigned long readRegister(uint8_t registerAddress);
    void writeRegister(uint8_t registerAddress, uint8_t registerValueW);



};

#endif