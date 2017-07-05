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
#include <ESP8266WebServer.h>
#include "WiFiConfig.h"
#include "WiFiConn.h"
#include "config.h"
#include "util.h"

ESP8266WebServer server(80);

/**
 * WiFi SSID to connect to. Updated dynamically from stored configuration.
 */
char ssid[33] = {0};

/**
 * WiFi PSK to connect with. Updated dynamically from stored configuration.
 */
char password[65] = {0};

/**
 * Configuration password (PSK) for WiFi AP mode.
 */
char config_pass[64] = {0};

/**
 * Global variable storing whether the WiFi is currently enabled or not.
 */
int wifiEnabled = 0;

/**
 * MAC address of the ESP8266 WiFi board
 */
uint8_t mac[6];                     // the MAC address of your Wifi shield

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

  loadLocalWiFiCredentials();
  
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

/* WiFi AP mode */
void setupWiFiAP() {
  WiFi.mode(WIFI_AP);

  String AP_NameString = F("MicSense ");
  String AP_Pass;
  
  if (strlen(config_pass) <= 0) {
    // Unsecured mode
    uint8_t ap_mac[6];
    WiFi.softAPmacAddress(ap_mac);
    String mac_addr = String(ap_mac[5], HEX) + String(ap_mac[4], HEX) + String(ap_mac[3], HEX) + String(ap_mac[2], HEX) + String(ap_mac[1], HEX) + String(ap_mac[0], HEX);
    AP_NameString += F("!! ");
    AP_NameString += mac_addr;
    AP_Pass = String(F(DEFAULT_CONFIG_PASS));
  } else {
    // Use part of our salt in our SSID!
    uint8_t salt[SALT_SIZE];
    SerialPrintStrLn("[scrambleBytesAdv] Reading salt...");
    getSalt(salt);
    AP_NameString += String(salt[5], HEX) + String(salt[4], HEX) + String(salt[3], HEX) + String(salt[2], HEX) + String(salt[1], HEX) + String(salt[0], HEX);
    AP_Pass = String(config_pass);
  }
  
  WiFi.softAP(AP_NameString.c_str(), AP_Pass.c_str());

  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  SerialPrintStrLn("To configure, visit the hotspot IP:");
  Serial.println(myIP);
}

void configAPModeHandleRootGET(const char *flash = NULL) {
  String flash_s;

  if (strlen(config_pass) <= 0) {
    flash_s += F("This seems to be your first time configuring MicSense. Upon submission, a WiFi password will be generated for you. Please have a notepad ready to jot it down after submission.");
  }
  
  if (flash != NULL) {
    flash_s += String(flash);
  }
  
  server.sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
  server.sendHeader(F("Pragma"), F("no-cache"));
  server.sendHeader(F("Expires"), F("-1"));
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");
  
  server.sendContent(F("<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'></head><body><form action='/' method='POST'><p>"));
  server.sendContent(flash_s);
  server.sendContent(F("</p><br>"));
  server.sendContent(F("<b><u>WiFi Network</u></b><br>"));
  server.sendContent(F("SSID:<input type='text' name='ssid'><br>"));
  server.sendContent(F("Password:<input type='text' name='psk'><br>"));
  server.sendContent(F("<b><u>MicSense Server</u></b><br>"));
  server.sendContent(F("Server (Hostname/IP only):<input type='text' name='server'><br>"));
  server.sendContent(F("Enable HTTPS?<input type='checkbox' name='https_enabled' value='yes'><br>"));
  server.sendContent(F("HTTPS Fingerprint:<input type='text' name='fingerprint'><br>"));
  server.sendContent(F("<b>Verify that the information you've entered above is correct, then press Submit.</b><br>"));
  server.sendContent(F("<input type='submit' name='submit' value='Submit'></form><br>"));
  server.sendContent(F("</body></html>"));
  DUMP_STACK_STATS();
  server.client().stop();
}

void configAPModeHandleRootPOST() {
  if (server.hasArg("ssid") && server.hasArg("psk") && server.hasArg("server") && server.hasArg("fingerprint")) {
    int use_https = 0;
    int redir_to_appass = 0;
    loadLocalWiFiCredentials();
    DUMP_STACK_STATS();
    if (strlen(config_pass) <= 0) {
      redir_to_appass = 1;
      saveLocalWiFiCredentials(server.arg("ssid").c_str(), server.arg("psk").c_str(), NULL);
    } else {
      // Preserve current config_pass
      saveLocalWiFiCredentials(server.arg("ssid").c_str(), server.arg("psk").c_str(), config_pass);
    }
    loadLocalWiFiCredentials();
    DUMP_STACK_STATS();
    
    if (server.hasArg("https_enabled") && (server.arg("https_enabled") == "yes")) {
      use_https = 1;
    }

    saveLocalServerCredentials(server.arg("server").c_str(), use_https, server.arg("fingerprint").c_str());
    DUMP_STACK_STATS();
    
    // Do we have a password set already?
    if (redir_to_appass == 1) {
      server.sendHeader("Location","/appass");
      server.sendHeader("Cache-Control","no-cache");
      server.send(301);
    } else {
      server.sendHeader("Location","/done");
      server.sendHeader("Cache-Control","no-cache");
      server.send(301);
    }
  } else {
    configAPModeHandleRootGET("Missing fields!");
  }
  DUMP_STACK_STATS();
}

void configAPModeHandleRoot() {
  if (server.method() == HTTP_GET) {
    configAPModeHandleRootGET();
  } else {
    configAPModeHandleRootPOST();
  }
}

void configAPModeHandleAPPass() {
  uint8_t salt[SALT_SIZE];
  String AP_NameString = F("MicSense ");
  String AP_Pass;
  SerialPrintStrLn("[configAPModeHandleAPPass] Reading salt...");
  getSalt(salt);
  AP_NameString += String(salt[5], HEX) + String(salt[4], HEX) + String(salt[3], HEX) + String(salt[2], HEX) + String(salt[1], HEX) + String(salt[0], HEX);
  AP_Pass = String(config_pass);

  SerialPrintStrLn("[configAPModeHandleAPPass] Displaying page...");

  server.sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
  server.sendHeader(F("Pragma"), F("no-cache"));
  server.sendHeader(F("Expires"), F("-1"));
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");
  
  server.sendContent(F("<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'></head><body>"));
  server.sendContent(F("<b><u>Configuration WiFi AP Name + Password</u></b><br>"));
  server.sendContent(F("This MicSense node has successfully been configured. As such, it will now be secured using a different SSID and password. Please note them below:<br>"));
  server.sendContent(F("<b>SSID:</b> <input type='text' style='font-family:monospace;' value='"));
  server.sendContent(AP_NameString);
  server.sendContent(F("' readonly><br>"));
  server.sendContent(F("<b>Password:</b> <input type='text' style='font-family:monospace;' value='"));
  server.sendContent(AP_Pass);
  server.sendContent(F("' readonly><br><br>"));
  server.sendContent(F("Once you are ready, click Continue.<br><br>"));
  server.sendContent(F("<a href='/done'>Continue</a>"));
  server.sendContent(F("</body></html>"));
  server.client().stop();
}

void configAPModeHandleDone() {
  server.sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
  server.sendHeader(F("Pragma"), F("no-cache"));
  server.sendHeader(F("Expires"), F("-1"));
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");
  
  server.sendContent(F("<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'></head><body>"));
  server.sendContent(F("<b><u>Done!</u></b><br><br>"));
  server.sendContent(F("In a moment, the MicSense node will reboot with your new configuration and you will be disconnected."));
  server.sendContent(F("</body></html>"));
  server.client().stop();
  delay(2000);
  ESP.restart();
}

void configAPMode() {
  bool ledState = false;
  unsigned long prevMillis = millis();

  SerialPrintStrLn("[configAPMode] Starting AP...");
  setupWiFiAP();

  SerialPrintStrLn("[configAPMode] Starting config...");
  
  /* Configure LEDs */
  digitalWrite(2, HIGH);
  
  server.on("/", configAPModeHandleRoot);
  server.on("/appass", configAPModeHandleAPPass);
  server.on("/done", configAPModeHandleDone);
  server.begin();

  while(1) {
    if (millis() - prevMillis > 1000) {
      if (ledState) {
        ledState = false;
        digitalWrite(2, HIGH);
      } else {
        ledState = true;
        digitalWrite(2, LOW);
      }
      prevMillis = millis();
    }
    server.handleClient();
  }
}

void configAPModeProbe() {
  unsigned long prevMillis = millis();
  bool enterConfigAPMode = false;

  SerialPrintStrLn("[configAPModeProbe] Waiting for GPIO0 to go low...");
  
  pinMode(0, INPUT);
  digitalWrite(2, LOW);

  while(1) {
    if (digitalRead(0) == LOW) {
      SerialPrintStrLn("[configAPModeProbe] Detected GPIO0 going low, starting config...");
      enterConfigAPMode = true;
      break;
    }
    if (millis() - prevMillis > 5000) {
      break;
    }
    delay(1);
  }
  digitalWrite(2, HIGH);
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);
  
  if (enterConfigAPMode) {
    configAPMode();
  }
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
