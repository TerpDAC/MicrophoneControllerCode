/**
 * @file   WiFiConn.cpp
 * @brief  WiFi connection functions
 *
 * Provides functions related to WiFi - setup, teardown, and
 * related control functions. WiFi is provided via interrupts on this
 * device. (This is hidden within the ESP8266 WiFi library.)
 * 
 * @author MicSense Team / Terrapin Development and Consulting
 * @date   May 2017
 */

/*====================================================================*/

#include <ESP8266WiFi.h>
#include "WiFiConfig.h"
#include "WiFiConn.h"
#include "util.h"

int value = 0;
const char* host = "www.example.com";

/**
 * Global variable storing whether the WiFi is currently enabled or not.
 */
int wifiEnabled = 0;

/**
 * MAC address of the ESP8266 WiFi board
 */
byte mac[6];                     // the MAC address of your Wifi shield

/**
 * Ask the ESP8266 to deep sleep for the specified amount of seconds.
 * 
 * Tell the ESP8266 to perform a deep sleep for a specified amount of
 * seconds. Deep sleep will completely reset the ESP8266 entirely, and
 * execution will restart at the beginning.
 * 
 * As a result of this behavior (due to not needing to save RAM), deep
 * sleep yields significant power savings, and is the lowest power
 * consumption possible for this chip.
 * 
 * To sleep without resetting the ESP8266 (at the cost of more power
 * consumption), see espSleep().
 * 
 * @param [in] sec  Number of seconds to perform the deep sleep.
 * @see espSleep()
 */
void espDeepSleep(uint32_t sec) {
  SerialPrintStr("Triggering deep sleep for ");
  Serial.print(sec);
  SerialPrintStrLn(" seconds...");
  wifiDisable();
  ESP.deepSleep(sec * 1000000);
}

/**
 * Ask the ESP8266 to sleep for the specified amount of seconds.
 * 
 * Tell the ESP8266 to perform a regular sleep for a specified amount
 * of seconds. Regular sleep will allow execution to continue from
 * where the sleep was called.
 * 
 * Regular sleep yields some power savings due to the CPU stopping,
 * and is a lower power consumption possible for this chip.
 * 
 * For a much lower power consumption (with the cost of resetting the
 * device). see espDeepSleep().
 * 
 * @param [in] sec  Number of seconds to perform the sleep.
 * @see espDeepSleep()
 */
void espSleep(uint32_t sec) {
  SerialPrintStr("Triggering sleep for ");
  Serial.print(sec);
  SerialPrintStrLn(" seconds...");
  wifiDisable();
  delay(sec * 1000);
}

/*====================================================================*/

/**
 * Connect to WiFi.
 * 
 * Using the current settings, connect to WiFi. This call currently
 * will block until a successful connection is made.
 */
void connectToWiFi() {
  WiFi.macAddress(mac);
  SerialPrintStr("MAC: ");
  Serial.print(mac[5],HEX);
  SerialPrintStr(":");
  Serial.print(mac[4],HEX);
  SerialPrintStr(":");
  Serial.print(mac[3],HEX);
  SerialPrintStr(":");
  Serial.print(mac[2],HEX);
  SerialPrintStr(":");
  Serial.print(mac[1],HEX);
  SerialPrintStr(":");
  Serial.println(mac[0],HEX);
  
  Serial.println();
  Serial.println();

  Serial.println();
  SerialPrintStrLn("ESP8266 stored WiFi credentials:");
  SerialPrintStr(" * SSID: ");
  Serial.println(WiFi.SSID());

  SerialPrintStr(" * PSK: ");
  Serial.println(WiFi.psk());
  Serial.println();

  SerialPrintStr("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    SerialPrintStr(".");
    
    if (WiFi.status() == WL_SCAN_COMPLETED) {
      SerialPrintStrLn("Scan completed");
    }

    if (WiFi.status() == WL_CONNECTION_LOST) {
      SerialPrintStrLn("Connection lost");
    }
    
    if (WiFi.status() == WL_IDLE_STATUS) {
      SerialPrintStrLn("Idle");
    }

    if (WiFi.status() == WL_NO_SSID_AVAIL) {
      SerialPrintStrLn("Could not find SSID");
    }
    
    if (WiFi.status() == WL_CONNECT_FAILED) {
      SerialPrintStrLn("Failed to connect to WiFi!");
    }
  }
 
  SerialPrintStrLn("");
  SerialPrintStrLn("WiFi connected");  
  SerialPrintStrLn("IP address: ");
  Serial.println(WiFi.localIP());
  
  wifiEnabled = 1;
}

/* Client timeout handler */
bool waitForClient(WiFiClient client) {
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      SerialPrintStrLn(">>> Client Timeout !");
      client.stop();
      return false;
    }
  }
  return true;
}

/* Test WiFi by grabbing a webpage. */
void wifiTest() {
  SerialPrintStr("performing wifi test in 5s...");
  delay(5000);
  ++value;
 
  SerialPrintStr("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    SerialPrintStrLn("connection failed");
    return;
  }
  
  // We now create a URI for the request
  //String url = "/testwifi/index.html";
  String url = "/";
  SerialPrintStr("Requesting URL: ");
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
  SerialPrintStrLn("closing connection");
}

/* Disable WiFi. */
void wifiDisable() {
  SerialPrintStrLn("Disconnecting from WiFi and disabling...");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(100);
  WiFi.forceSleepBegin();
  delay(100);
  wifiEnabled = 0;
}

/* Enable WiFi. */
void wifiEnable() {
  SerialPrintStrLn("Enabling WiFi...");
  WiFi.forceSleepWake();
  delay(100);
  WiFi.mode(WIFI_STA);
  delay(100);
  /* Note that wifiEnabled = 1 is set within connectToWiFi(). */
  connectToWiFi();
}

/* Return if WiFi is enabled or not. */
int getWiFiEnabled() {
  return wifiEnabled;
}
