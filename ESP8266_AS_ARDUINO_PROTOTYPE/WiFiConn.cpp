#include <ESP8266WiFi.h>
#include "config.h"
#include "WiFiConn.h"

int value = 0;
const char* host = "www.example.com";
int wifiEnabled = 0;

void connectToWiFi() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  wifiEnabled = 1;
}

void wifiTest() {
  Serial.print("performing wifi test in 5s...");
  delay(5000);
  ++value;
 
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  //String url = "/testwifi/index.html";
  String url = "/";
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}

void wifiDisable() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(100);
  WiFi.forceSleepBegin();
  delay(100);
  wifiEnabled = 0;
}

void wifiEnable() {
  WiFi.forceSleepWake();
  delay(100);
  WiFi.mode(WIFI_STA);
  delay(100);
  connectToWiFi();
}

int getWiFiEnabled() {
  return wifiEnabled;
}
