#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "html.h"
#include "ads1256.h"

//EJEMPLOS HTML https://circuits4you.com/2016/12/16/esp8266-web-server-html/
ESP8266WebServer server(80);

bool shipping_status=false;
bool reset_status=false;

const char* psid = "LowiD7E0";
const char* password = "B87BLCFKWEDC3M";

//const char* psid = "vodafoneD5D8";
//const char* password = "Fedaropa4";

//const char* psid = "AndroidAP0377";
//const char* password = "31fd356818b5";

void handleRoot() {
  server.send(8000, "text/plain", mesage1);
  shipping_status=true;
}
void menu(){
  server.send(8000, "text/html", menuHTML);
}
void wifi(){
  server.send(8000, "text/html", wifiHTML);
}
void Reset(){
  server.send(800, "text/html", "Reseteando...");
  reset_status=true;
}
void handleLogin() {     
  if( ! server.hasArg("ssid") || ! server.hasArg("password") 
      || server.arg("ssid") == NULL || server.arg("password") == NULL) { // If the POST request doesn't have username and password data
    server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }
  else{ // If both the username and the password are correct
    //ssid=server.arg("ssid");
    //password=server.arg("password");
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
  Serial.println("");

  //Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", menu);
  server.on("/data", handleRoot);
  server.on("/reset", Reset);
  server.on("/wifi", wifi);
  server.on("/login", HTTP_POST, handleLogin);

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


