//Sending:
#include<SoftwareSerial.h>
SoftwareSerial SUART (2, 3);

float rpm_val = 0.27907;
float rpm_val2 = 0.97867564534;

void setup()
{
  Serial.begin(115200);
  SUART.begin(115200);
}

void loop()
{
  Serial.println("sending..");
  SUART.println(rpm_val, 5);
  //delay(500);
  //SUART.println(rpm_val2, 11);
  Serial.println(rpm_val, 5);
  //Serial.println(rpm_val2, 11);
  delay(5000);
}
