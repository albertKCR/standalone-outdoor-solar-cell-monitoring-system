#include "ESP8266WiFi.h"
#include "HTTPSRedirect.h"
#include <SoftwareSerial.h>

SoftwareSerial arduinoSUART(4, 5);  //RX, TX
HTTPSRedirect* client = nullptr;
int stringCounter = 0;

//URL to access the google sheet
const char* GScriptId   = "AKfycbxrtin5P-VncJmSKlx2dsphOA4bplVVLjcx_A4ZB8jCMshNo4t5QQuAGFWkii0A-NxY";
String payload_base =  "{\"command\": \"append_row\", \"sheet_name\": \"Sheet1\", \"values\": ";
String payload = "";
const char* host = "script.google.com";
const int httpsPort = 443;
String url = String("/macros/s/") + GScriptId + "/exec?cal";

//array that store the received values of current and voltage
String toSendData[40];

void setup() {
  Serial.begin(115200);
  arduinoSUART.begin(115200);
  
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  delay(100);
  WiFi.begin("UTFPR-Projetos", "pNU9cnpPZ$tM");
  Serial.println(WiFi.macAddress());

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  // --- HTTPS Redirect Setup ---
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  Serial.print("Connecting to Google...");

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
  // ---
}

void loop() {
  if(arduinoSUART.available()>0){ //check if arduino sended data

    // --- HTTPS protocol ---
    static bool flag = false;
    if (!flag){
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
    // ---

    stringCounter = 0;
    for (int i = 0; i < 40; i++){
      if(!(arduinoSUART.available()>0)) break; //if arduino stop sending data, breaks the for loop
      String receivedData = arduinoSUART.readString(); //read the string send by arduino
      Serial.println(receivedData);
      toSendData[i] = receivedData;
      stringCounter++;
    }

    // the payload will be constructed according to the received data
    payload = payload_base + "\"" + toSendData[0];
    for (int i = 1; i < stringCounter; i++){
      payload = payload + "," + toSendData[i];
    }
    payload = payload + "\"}";

    //payload = payload_base + "\"" + toSendData[0] + "," + toSendData[1] + "," + toSendData[2] + "," + toSendData[3] + "," + toSendData[4] + "," + toSendData[5] + "," + toSendData[6] + "," + toSendData[7] + "," + toSendData[8] + "," + toSendData[9] +  "," + toSendData[10] + "," + toSendData[11] + "," + toSendData[12] + "," + toSendData[13] + "," + toSendData[14] + "," + toSendData[15] + "," + toSendData[16] + "," + toSendData[17] + "," + toSendData[18] + "," + toSendData[19] +  "," + toSendData[20] +  "," + toSendData[21] +  "," + toSendData[22] +  "," + toSendData[23] +  "," + toSendData[24] +  "," + toSendData[25] +  "," + toSendData[26] +  "," + toSendData[27] +  "," + toSendData[28] +  "," + toSendData[29] +  "," + toSendData[30] +  "," + toSendData[31] +  "," + toSendData[32] +  "," + toSendData[33] +  "," + toSendData[34] +  "," + toSendData[35] +  "," + toSendData[36] +  "," + toSendData[37] +  "," + toSendData[38] +  "," + toSendData[39] + "\"}";
    Serial.println("Sending...");
  
    if (client->POST(url, host, payload)){ //Send the data through the google API
      Serial.println(" [OK]");
    }
    else {
      Serial.println("[Error]");
    }
  }
}
