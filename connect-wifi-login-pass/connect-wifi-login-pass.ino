#include  <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <WiFiClientSecure.h>

SoftwareSerial mySUART(4, 5);

char myRPM[20];

//----------------Variables and objects to google sheets------------------------
// The ID below comes from Google Sheets.
// Towards the bottom of this page, it will explain how this can be obtained
String GAS_ID = "AKfycbzQGtgkHxBztnu_oPnCI72reLqt20_yLVwUFseytrDjD1JHvKt-fiFaujMBlhIcWaxS";
String readString;
const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure client;
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
void sendData(String x, String y);
//--------------------------Internet Connection--------------
extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
#include "c_types.h"
}

// SSID to connect to
char ssid[] = "UTFPR-ALUNO";
char username[] = "a2263580";
char identity[] = "a2263580";
char password[] = "xx";


uint8_t target_esp_mac[6] = {0x24, 0x0a, 0xc4, 0x9a, 0x58, 0x28};


void setup() {
  mySUART.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.begin(9600);
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

  //----------------------------------------
   
}


void loop() {
  if(mySUART.available()>0){
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
    sendData(myRPM1, x);
  }
  delay(300);

}

void sendData(String x, String y)
{
  client.setInsecure();
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
  Serial.println("certificate matches");
  } else {
  Serial.println("certificate doesn't match");
  }

  String url = "/macros/s/" + GAS_ID + "/exec?voltage=" + x + "&current=" + y;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
  String line = client.readStringUntil('\n');
  if (line == "\r") {
    Serial.println("headers received");
    break;
  }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
  Serial.println("esp8266/Arduino CI successfull!");
  } else {
  Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}
