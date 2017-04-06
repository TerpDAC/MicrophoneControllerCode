#include <ESP8266WiFi.h>
#include "WiFiConfig.h"
#include "WiFiConn.h"
#include "Time.h"
#include "config.h"

int value = 0;
const char* host = "www.example.com";
int wifiEnabled = 0;
byte mac[6];                     // the MAC address of your Wifi shield

void espDeepSleep(uint32_t sec) {
  Serial.print("Triggering deep sleep for ");
  Serial.print(sec);
  Serial.println(" seconds...");
  wifiDisable();
  ESP.deepSleep(sec * 1000000);
}

void espSleep(uint32_t sec) {
  Serial.print("Triggering sleep for ");
  Serial.print(sec);
  Serial.println(" seconds...");
  wifiDisable();
  delay(sec * 1000);
}

void connectToWiFi() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    
    if (WiFi.status() == WL_SCAN_COMPLETED) {
      Serial.println("Scan completed");
    }

    if (WiFi.status() == WL_CONNECTION_LOST) {
      Serial.println("Connection lost");
    }
    
    if (WiFi.status() == WL_IDLE_STATUS) {
      Serial.println("Idle");
    }

    if (WiFi.status() == WL_NO_SSID_AVAIL) {
      Serial.println("Could not find SSID");
    }
    
    if (WiFi.status() == WL_CONNECT_FAILED) {
      Serial.println("Failed to connect to WiFi!");
    }
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
  
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
  Serial.println("Disconnecting from WiFi and disabling...");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(100);
  WiFi.forceSleepBegin();
  delay(100);
  wifiEnabled = 0;
}

void wifiEnable() {
  Serial.println("Enabling WiFi...");
  WiFi.forceSleepWake();
  delay(100);
  WiFi.mode(WIFI_STA);
  delay(100);
  connectToWiFi();
}

int getWiFiEnabled() {
  return wifiEnabled;
}

void submitSum(uint32_t curTime, long totalSampleCount, long highTotalCount, long midTotalCount, byte[] address) {
  Serial.println("Submit code goes here!");

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    Serial.print("GET /datasubmit?d=");
    Serial.print(now());
    Serial.print(",");
    Serial.print(highTotalCount);
    Serial.print(",");
    Serial.print(midTotalCount);
    Serial.print(",");
    Serial.print(totalSampleCount - (highTotalCount + midTotalCount));

    Serial.print("&id=");
    Serial.print(address[5], HEX);
    Serial.print(address[4], HEX);
    Serial.print(address[3], HEX);
    Serial.print(address[2], HEX);
    Serial.print(address[1], HEX);
    Serial.print(address[0], HEX);

    Serial.println(" HTTP/1.1");
    Serial.print("Host: ");
    Serial.println(server);
    Serial.println("Connection: close");
    Serial.println();
    // Make a HTTP request:
    client.print("GET /datasubmit?d=");
    client.print(now());
    client.print(",");
    client.print(highTotalCount);
    client.print(",");
    client.print(midTotalCount);
    client.print(",");
    client.print(totalSampleCount - (highTotalCount + midTotalCount));

    client.print("&id=");
    client.print(address[5], HEX);
    client.print(address[4], HEX);
    client.print(address[3], HEX);
    client.print(address[2], HEX);
    client.print(address[1], HEX);
    client.print(address[0], HEX);

    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close\r\n\r\n");

    Serial.println("all data sent");
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    
    Serial.println();
    Serial.println("closing connection");
  }
}
