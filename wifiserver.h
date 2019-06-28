#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "html.h"
#include "ads1256.h"


#include<string.h>
#include <EEPROM.h>

//EJEMPLOS HTML https://circuits4you.com/2016/12/16/esp8266-web-server-html/
ESP8266WebServer server(80);

String  fw;

union Float_Byte
{
float datoF;
byte  datoB[4];
} offset_Voltage,offset_Current,gain_Voltage,gain_Current;

char name_device[10];
byte PGAvoltage;
byte PGAcurrent;
byte DataRate;

bool shipping_status=false;
bool reset_status=false;
bool calibrate_status=false;

const char* psid = "LowiD7E0";
const char* password = "B87BLCFKWEDC3M";

char* ssid_1;
char* password_1;
//const char* psid = "vodafoneD5D8";
//const char* password = "Fedaropa4";

//const char* psid = "AndroidAP0377";
//const char* password = "31fd356818b5";

void handleRoot() {
  server.send(8000, "text/plain", mesage1);
  shipping_status=true;
}

void Data() {
  server.send(8000, "text/plain", mesage1);
}

void menu(){
  server.send(8000, "text/html", menuHTML);
}
void wifi(){
  server.send(8000, "text/html", wifiHTML);
}
void setting() {
  server.send(8000, "text/html", settingHTML);
}
void Reset(){
  server.send(800, "text/html", "Reseteando...");
  reset_status=true;
}
void calibrate(){
  server.send(8000, "text/html", calibrateHTML);
}
void setsetting(){
  server.arg("name_device").toCharArray(name_device, 10);
  PGAvoltage=server.arg("PGAvoltage").toInt();
  PGAcurrent=server.arg("PGAcurrent").toInt();
  DataRate=server.arg("DataRate").toInt();
  for(int i=0; i<10; i++){
    EEPROM.put(116+i, name_device[i]);
  } 
  EEPROM.put(126, PGAvoltage);
  EEPROM.put(127, PGAcurrent);
  EEPROM.put(128, DataRate);
  EEPROM.commit();
  reset_status=true;
}
void setOffsetVoltage(){
  if(server.hasArg("offset_Voltage")){
    fw = server.arg("offset_Voltage");
    offset_Voltage.datoF=fw.toFloat();
    for(int i=0; i<4; i++){
      EEPROM.put(i, offset_Voltage.datoB[i]);
    }
  }  
  if(server.hasArg("offset_Current")){  
    fw = server.arg("offset_Current");
    offset_Current.datoF=fw.toFloat();
    for(int i=0; i<4; i++){
      EEPROM.put(4+i, offset_Current.datoB[i]);
    }    
  }  
  if(server.hasArg("gain_Voltage")){  
    fw = server.arg("gain_Voltage");
    gain_Voltage.datoF=fw.toFloat();
    for(int i=0; i<4; i++){
      EEPROM.put(8+i, gain_Voltage.datoB[i]);
    }    
  }  
  if(server.hasArg("gain_Current")){  
    fw = server.arg("gain_Current");
    gain_Current.datoF=fw.toFloat();
    for(int i=0; i<4; i++){
      EEPROM.put(12+i, gain_Current.datoB[i]);
    }    
  }  
  EEPROM.commit();
  calibrate_status=true;
  server.send(8000, "text/html", calibrateHTML);
}
void handleLogin() {     
  if( ! server.hasArg("ssid") || ! server.hasArg("password") 
      || server.arg("ssid") == NULL || server.arg("password") == NULL) { // If the POST request doesn't have username and password data
    server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }
  else{ // If both the username and the password are correct
    server.arg("ssid").toCharArray(ssid_1, 50);
    server.arg("password").toCharArray(password_1, 50);
    //sprintf(ssid_1, "%s", server.arg("ssid").toCharArray(copy, 50));
    //sprintf(password_1, "%s", server.arg("password").toCharArray(copy, 50));
    for(int i=0; i<50; i++){
      EEPROM.put(16+i, ssid_1[i]);
      EEPROM.put(66+i, password_1[i]);
    }
    EEPROM.commit();
    reset_status=true;
  }
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void initserver(){  
  WiFi.mode(WIFI_STA);
  WiFi.begin(psid, password);
  //WiFi.begin(ssid_1, password_1);
  Serial.println("");

  //Wait for connection
  while ((WiFi.status()!= WL_CONNECTED)) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", menu);
  server.on("/data", Data);
  server.on("/senddata", handleRoot);
  server.on("/reset", Reset);
  server.on("/wifi", wifi);
  server.on("/calibrate", calibrate);
  server.on("/setting", setting);
  server.on("/setsetting", setsetting);
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/setcalibrate", HTTP_POST, setOffsetVoltage);

  server.on("/inline", []() {
  server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

}

void initPA(){
  WiFi.mode(WIFI_AP);
  WiFi.softAP("wattmeter");

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}
void chargeData(){
  offset_Voltage.datoF = 0.0;
  offset_Current.datoF = 0.0;
  gain_Voltage.datoF = 0.0;
  gain_Current.datoF = 0.0;
  PGAvoltage= 0;
  PGAcurrent= 0;
  DataRate= 0;
  sprintf(name_device,"");
  
  for(int i=0; i<4; i++){
    EEPROM.get(i, offset_Voltage.datoB[i]);
  }
  for(int i=0; i<4; i++){
    EEPROM.get(4+i, offset_Current.datoB[i]);
  }
  for(int i=0; i<4; i++){
    EEPROM.get(8+i, gain_Voltage.datoB[i]);
  }
  for(int i=0; i<4; i++){
    EEPROM.get(12+i, gain_Current.datoB[i]);
  }
  
  for(int i=0; i<10; i++){
    EEPROM.get(116+i, name_device[i]);
  } 
  EEPROM.get(126, PGAvoltage);
  EEPROM.get(127, PGAcurrent);
  EEPROM.get(128, DataRate);
}


