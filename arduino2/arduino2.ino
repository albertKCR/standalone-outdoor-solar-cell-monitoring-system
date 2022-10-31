//Sending:
#include<SoftwareSerial.h>
SoftwareSerial SUART (2, 3);

double rpm_val = 0.123451234512;
double rpm_val2 = 0.97867564534;

void setup()
{
  Serial.begin(115200);
  SUART.begin(115200);
}

void loop()
{
  Serial.println("sending..");
  Serial.println(rpm_val, 12);
  SUART.println(rpm_val, 15);
  //delay(500);
  //SUART.println(rpm_val2, 11);
  Serial.println(rpm_val, 15);
  Serial.println(rpm_val2, 15);
  delay(5000);
}
