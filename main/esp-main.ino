#include "ESP8266WiFi.h"
#include "HTTPSRedirect.h"
#include <SoftwareSerial.h>

SoftwareSerial mySUART(4, 5);  //D2, D1

HTTPSRedirect* client = nullptr;
const char* GScriptId   = "AKfycbzJ__WvYpeAAznt4LC4DscsHw7U8C2LHCng5COQcLsHr3VXBwalZAJjD4QDmNQG19Bw";
//-----------------------------------------------------------------


String payload_base =  "{\"command\": \"append_row\", \"sheet_name\": \"Sheet1\", \"values\": ";
String payload = "";

const char* host        = "script.google.com";
const int   httpsPort   = 443;
String      url         = String("/macros/s/") + GScriptId + "/exec?cal";
int voltage = 0;
int current = 1;
char myRPM[20];
void setup() {

  Serial.begin(115200);
  mySUART.begin(115200);
  Serial.println("aaaaa");

  // Set WiFi to station mode and disconnect from an AP if it was previously connected

  delay(100);
  WiFi.begin("AP18", "zanella123");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  //HTTPS Redirect Setup
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  Serial.print("Conectando ao Google...");

  bool flag = false;
  for (int i = 0; i < 5; i++)
  {
    int retval = client->connect(host, httpsPort);
    if (retval == 1)
    {
      flag = true;
      Serial.println("[OK]");
      break;
    }
    else
      Serial.println("[Error]");
  }
  if (!flag)
  {
    Serial.print("[Error]");
    Serial.println(host);
    return;
  }
  delete client;
  client = nullptr;
  Serial.println("Setup done");
}

void loop() {

  if (WiFi.status() != WL_CONNECTED){
    mySUART.println(0);
  }
  else{
  mySUART.println(1);
  Serial.println(WiFi.localIP());

  if(mySUART.available()>0){
  static bool flag = false;
  if (!flag)
  {
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }
  if (client != nullptr) {
    if (!client->connected()) {
      client->connect(host, httpsPort);
    }
  }
  else {
    Serial.println("[Error]");
  }
  byte m = mySUART.readBytesUntil('\n', myRPM, 20);
  int l = m;
  myRPM[l] = '\0';
  Serial.println(myRPM);
  float rpm = atof(myRPM);
  Serial.println(rpm, 12);
  //Serial.println(myRPM);
  String myRPM1(rpm, 12);
  Serial.println(myRPM1);

  String x = "0.298767123456123";
  delay(500);
  payload = payload_base + "\"" + x + "," + myRPM1 + "\"}";

  Serial.println("Enviando...");
  
  if (client->POST(url, host, payload)) {
    Serial.println(" [OK]");
  }
  else {
    Serial.println("[Error]");
  }
  }

  }


  delay(1000);
}
