#include "ADS1256.h"

ADS1256::ADS1256()
{
 
    resolution = 8388608.; // 2^23-1

    // this needs to match the setting in the ADC init function in the library tab
    Gain = 1.; // be sure to have a period

    vRef =5.0478; //5.0458; // reference voltage

    // we'll calculate this in setup
    bitToVolt = resolution * Gain / vRef;

    DRDY_state = HIGH;
    
}

void ADS1256 :: begin(){

    //Serial.begin(115200);
    SPI.begin();

    pinMode(ADS_CS_PIN, OUTPUT);
    pinMode(ADS_RDY_PIN, INPUT);
    pinMode(ADS_RST_PIN, OUTPUT);

    for(int i= 0; i<7; i++){

        eep.get((60000 +(i*4)) , correction[i]);

    }

 
    digitalWrite(ADS_RST_PIN, LOW);
    delay(10); // LOW at least 4 clock cycles of onboard clock. 100 microsecons is enough
    digitalWrite(ADS_RST_PIN, HIGH); // now reset to deafult values

    delay(1000);

    //now reset the ADS
    Reset();
    delay(2000);

    SetRegisterValue(MUX, MUX_RESET); // set to read channel(0,1)

    SetRegisterValue(ADCON, PGA_1);

    SetRegisterValue(DRATE, DR_100);

    delay(2000);

    SendCMD(SELFCAL); //send the calibration command

    delay(5);

}


float ADS1256 :: returnCurrent(){
  return OutputCurrent;
}
float ADS1256 :: returnVoltage(){
  return OutputVoltage;
}

void ADS1256 :: average(){
  double aux1 = 0.0;
  double aux2 = 0.0;

  for (int i = 0; i < 10; i++) {
 
    read_two_values();
    aux1 += val1;
    aux2 += val2;

  }

  aux1 /= 10.0;
  aux2 /= 10.0;

  val1 = aux1;
  val2 = aux2;

}


void ADS1256 :: average(int value){
  double aux1 = 0.0;
  double aux2 = 0.0;

  for (int i = 0; i < value; i++) {
 
    read_two_values();
    aux1 += val1;
    aux2 += val2;

  }

  aux1 /= value;
  aux2 /= value;

  val1 = aux1;
  val2 = aux2;

}



void ADS1256 :: conversion(float res){ // the final value depends on resistance value
 
    average();

    switch (rangeCounter)
    {
    case 0 :
      val2 += correction[0];
      break;

    case 1 : 
      //val1 += 0.0082;
      val2 += correction[1];
      break;

    case 2 :
      //val1 += 0.0077;
      val2 += correction[2];
      break;

    case 3 :
      //val1 += 0.0246;
      val2 += correction[3];
      break;  

    case 4 :
      //val1 += 0.0082;
      val2 += correction[4];
      break;  
      
    case 5 :
      //val1 += 0.0215;
      val2 += correction[5];
      break;  

    case 6 :
      //val1 += 0.0213;
      val2 += correction[6] ;
      break;      

    }

 

    OutputCurrent  = (val2/(100.0 * res)) ;
    //OutputCurrent  = (val2/( res))*1000 ;


    OutputVoltage  = val1;

    diffe = val1 - (val2/100);

    if(abs(val2) > 2.40) {
        top_overflow = true; 
        bottom_overflow = false;
    }

    else if (abs(val2) < 0.230){
        top_overflow = false;
        bottom_overflow = true;
    }

    else {
        top_overflow = false;
        bottom_overflow = false;
    }

}

bool ADS1256 :: botOV(){return bottom_overflow;} 

bool ADS1256 :: topOV(){return top_overflow;}




void ADS1256 :: DRDY_Interuppt() { // ISR FUNCTION
  DRDY_state = LOW;
} 

void ADS1256 :: Reset() {
  SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1)); // initialize SPI with  clock, MSB first, SPI Mode1
  digitalWriteFast(ADS_CS_PIN, LOW);
  delayMicroseconds(10);
  SPI.transfer(RESET); //Reset
  delay(2); //Minimum 0.6ms required for Reset to finish.
  SPI.transfer(SDATAC); //Issue SDATAC
  delayMicroseconds(100);
  digitalWriteFast(ADS_CS_PIN, HIGH);
  SPI.endTransaction();
}


void ADS1256 :: SetRegisterValue(uint8_t regAdress, uint8_t regValue) {

  uint8_t regValuePre = GetRegisterValue(regAdress);
  if (regValue != regValuePre) {
    //digitalWrite(_START, HIGH);
    delayMicroseconds(10);
    waitforDRDY();
    SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1)); // initialize SPI with SPI_SPEED, MSB first, SPI Mode1
    digitalWriteFast(ADS_CS_PIN, LOW);
    delayMicroseconds(10);
    SPI.transfer(WREG | regAdress); // send 1st command byte, address of the register
    SPI.transfer(0x00);   // send 2nd command byte, write only one register
    SPI.transfer(regValue);         // write data (1 Byte) for the register
    delayMicroseconds(10);
    digitalWriteFast(ADS_CS_PIN, HIGH);
    //digitalWrite(_START, LOW);
    Serial.println(regValue);
    Serial.println(GetRegisterValue(regAdress));
    if (regValue != GetRegisterValue(regAdress)) {   //Check if write was succesfull
      Serial.print("Write to Register 0x");
      Serial.print(regAdress, HEX);
      Serial.println(" failed!");
    }
    else {
      Serial.println("success");
    }
    SPI.endTransaction();

  }

}


  long ADS1256 :: GetRegisterValue(uint8_t regAdress) {
    uint8_t bufr;
    digitalWriteFast(ADS_CS_PIN, LOW);
    delayMicroseconds(10);
    waitforDRDY();
    SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1));
    SPI.transfer(RREG | regAdress); // send 1st command byte, address of the register
    SPI.transfer(0x00);     // send 2nd command byte, read only one register
    delayMicroseconds(10);
    bufr = SPI.transfer(NOP); // read data of the register
    delayMicroseconds(10);
    digitalWriteFast(ADS_CS_PIN, HIGH);
    //digitalWrite(_START, LOW);
    SPI.endTransaction();
    return bufr;

}



void ADS1256 :: SendCMD(uint8_t cmd) {
  waitforDRDY();
  SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1)); // initialize SPI with 4Mhz clock, MSB first, SPI Mode0
  digitalWriteFast(ADS_CS_PIN, LOW);
  delayMicroseconds(10);
  SPI.transfer(cmd);
  delayMicroseconds(10);
  digitalWriteFast(ADS_CS_PIN, HIGH);
  SPI.endTransaction();
}


void ADS1256 :: waitforDRDY() {
  while (DRDY_state) {
   if(Serial.available());
  }
  noInterrupts();
  DRDY_state = HIGH;
  interrupts();
}




void ADS1256 :: read_two_values() {
  //datasheet page 21 at the bottom gives the timing
  int32_t adc_val1 = 0x00;
  int32_t adc_val2 = 0x00;

  waitforDRDY(); // Wait until DRDY is LOW
  SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1));
  digitalWriteFast(ADS_CS_PIN, LOW); //Pull SS Low to Enable Communications with ADS1247
  //delayMicroseconds(5); // RD: Wait 25ns for ADC12xx to get ready

  //now change the mux register
  SPI.transfer(WREG | MUX); // send 1st command byte, address of the register
  SPI.transfer(0x00);   // send 2nd command byte, write only one register
  //SPI.transfer(0x23);     //pins registers 2 and 3
  SPI.transfer(0x23); // channel 4 negative and 5 positive


  //now we need to sync
  //need to delay by 4x SPI clock = 2.35 uS (t1)
  //to be safe 5 uS
  delayMicroseconds(2);
  SPI.transfer(SYNC);

  //again delay by t1
  delayMicroseconds(5);
  //send wakeup
  SPI.transfer(WAKEUP);

  //then delay one more time by t1 before rdata
  delayMicroseconds(1);

  SPI.transfer(RDATA); //Issue RDATA
  delayMicroseconds(7);

  //This is the reading in the Data register from whatever the mux was set to the last
  //time this function was called. By default, it is configured to leave that value at 0
  adc_val1 |= SPI.transfer(NOP);
  //delayMicroseconds(10);
  adc_val1 <<= 8;
  adc_val1 |= SPI.transfer(NOP);
  //delayMicroseconds(10);
  adc_val1 <<= 8;
  adc_val1 |= SPI.transfer(NOP);
  //delayMicroseconds(5);

  //now wait for the next dataready
  waitforDRDY(); // Wait until DRDY is LOW
  //delayMicroseconds(5);

  //now change the mux register back to 0 so we left things how we found them
  SPI.transfer(WREG | MUX); // send 1st command byte, address of the register
  SPI.transfer(0x00);   // send 2nd command byte, write only one register
  SPI.transfer(0x67);     //pins registers 2 and 3

  //now we need to sync
  //need to delay by 4x SPI clock = 2.35 uS (t1)
  //to be safe 5 uS
  delayMicroseconds(2);
  SPI.transfer(SYNC);

  //again delay by t1
  delayMicroseconds(5);
  //send wakeup
  SPI.transfer(WAKEUP);

  //then delay one more time by t1 before rdata
  delayMicroseconds(1);

  SPI.transfer(RDATA); //Issue RDATA
  delayMicroseconds(7);
  //this should now be the value from the mux change we just did (0,1 to 2,3)
  adc_val2 |= SPI.transfer(NOP);
  //delayMicroseconds(10);
  adc_val2 <<= 8;
  adc_val2 |= SPI.transfer(NOP);
  //delayMicroseconds(10);
  adc_val2 <<= 8;
  adc_val2 |= SPI.transfer(NOP);
  //delayMicroseconds(5);
  //this is the value for the

  digitalWriteFast(ADS_CS_PIN, HIGH);
  SPI.endTransaction();

  if (adc_val1 > 0x7fffff) { //if MSB == 1
    adc_val1 = adc_val1 - 16777216; //do 2's complement, keep the sign this time!
  }

  if (adc_val2 > 0x7fffff) { //if MSB == 1
    adc_val2 = adc_val2 - 16777216; //do 2's complement, keep the sign this time!
  }

  val1 = (adc_val1/bitToVolt) - 0.5;

  val2 = (adc_val2/bitToVolt) -2.5;
}