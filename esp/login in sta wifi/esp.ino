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

extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
#include "c_types.h"
}

// SSID to connect to
char ssid[] = "UTFPR-ALUNO";
char username[] = "a2263580";
char identity[] = "a2263580";
char password[] = "***";


uint8_t target_esp_mac[6] = {0x24, 0x0a, 0xc4, 0x9a, 0x58, 0x28};

void setup() {

  Serial.begin(9600);
  mySUART.begin(115200);
  Serial.println("aaaaa");

  // Set WiFi to station mode and disconnect from an AP if it was previously connected

  delay(100);
   WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.setDebugOutput(true);
  Serial.printf("SDK version: %s\n", system_get_sdk_version());
  Serial.printf("Free Heap: %4d\n",ESP.getFreeHeap());
  
  // Setting ESP into STATION mode only (no AP mode or dual mode)
  wifi_set_opmode(STATION_MODE);

  struct station_config wifi_config;

  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char*)wifi_config.ssid, ssid);
  strcpy((char*)wifi_config.password, password);

  wifi_station_set_config(&wifi_config);
  wifi_set_macaddr(STATION_IF,target_esp_mac);
  

  wifi_station_set_wpa2_enterprise_auth(1);

  // Clean up to be sure no old data is still inside
  wifi_station_clear_cert_key();
  wifi_station_clear_enterprise_ca_cert();
  wifi_station_clear_enterprise_identity();
  wifi_station_clear_enterprise_username();
  wifi_station_clear_enterprise_password();
  wifi_station_clear_enterprise_new_password();
  
  wifi_station_set_enterprise_identity((uint8*)identity, strlen(identity));
  wifi_station_set_enterprise_username((uint8*)username, strlen(username));
  wifi_station_set_enterprise_password((uint8*)password, strlen((char*)password));

  
  wifi_station_connect();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

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

  /*if (WiFi.status() != WL_CONNECTED){
    mySUART.println(0);
  }
  else{*/
  //mySUART.println(1);
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
  payload = payload_base + "\"" + 0.998 + "," + 0.999 + "\"}";

  Serial.println("Enviando...");
  
  if (client->POST(url, host, payload)) {
    Serial.println(" [OK]");
  }
  else {
    Serial.println("[Error]");
  }
  }

  //}


  delay(1000);
}
