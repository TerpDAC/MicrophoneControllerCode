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
#include <ESP8266HTTPClient.h>
#include "WiFiConfig.h"
#include "WiFiConn.h"
#include "Time.h"
#include "config.h"
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
  Serial.print("Triggering deep sleep for ");
  Serial.print(sec);
  Serial.println(" seconds...");
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
  Serial.print("Triggering sleep for ");
  Serial.print(sec);
  Serial.println(" seconds...");
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
  
  wifiEnabled = 1;
}

/* Client timeout handler */
bool waitForClient(WiFiClient client) {
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return false;
    }
  }
  return true;
}

/* Test WiFi by grabbing a webpage. */
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

/* Disable WiFi. */
void wifiDisable() {
  Serial.println("Disconnecting from WiFi and disabling...");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(100);
  WiFi.forceSleepBegin();
  delay(100);
  wifiEnabled = 0;
}

/* Enable WiFi. */
void wifiEnable() {
  Serial.println("Enabling WiFi...");
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

/* Set the calibration, given the server output line. */
void setCalibration(String line, int update) {
  bool calibrationSet = false;

  // 012345678+ 
  // -------=---=
  // SenseOK:500:600
  
  Serial.println("----------- setCalibration -----------");
  
  if (line.indexOf("SenseOK") != -1) {
    String sense_part = line.substring(line.indexOf("SenseOK"));
    
    // Attempt to parse the line for any updates to the mid thresh
    // and high thresh
    if (sense_part.length() > 8) {
      size_t mid_thresh_str_end = sense_part.indexOf(":", 8);
      String mid_thresh_str;
      String high_thresh_str;
      int mid_thresh_tmp = -1;
      int high_thresh_tmp = -1;

#ifdef ENABLE_DEBUG_SERIAL_VERBOSE
      Serial.println(" ** setCalibration DEBUG ** ");
      Serial.print("debug setCalibration: mid_thresh_str_end = ");
      Serial.println(mid_thresh_str_end);
      Serial.println("sense_part = " + sense_part);
      Serial.println("sense_part[0] = " + String(sense_part.charAt(0)));
      Serial.println("sense_part[8] = " + String(sense_part.charAt(8)));
      Serial.println("sense_part[9] = " + String(sense_part.charAt(9)));
      Serial.println("sense_part[10] = " + String(sense_part.charAt(10)));
      Serial.println(" ** setCalibration DEBUG END ** ");
#endif

      // Did we find our colon?
      if (mid_thresh_str_end != std::string::npos) {
        // Split the strings up!
        mid_thresh_str = sense_part.substring(8, mid_thresh_str_end);
        high_thresh_str = sense_part.substring(mid_thresh_str_end + 1, sense_part.length());
        mid_thresh_tmp = mid_thresh_str.toInt();
        high_thresh_tmp = high_thresh_str.toInt();
        if (mid_thresh_tmp != 0) mid_thresh = mid_thresh_tmp; else Serial.println("Invalid argument to mid thresh: " + mid_thresh_str + '\n');
        if (high_thresh_tmp != 0) high_thresh = high_thresh_tmp; else Serial.println("Invalid argument to high thresh: " + high_thresh_str + '\n');
        calibrationSet = true;
        Serial.println("Calibration set successfully!");
        DPRINT("Calibration set successfully!\n");
      }
    }
  }

  if (!calibrationSet) {
    if (update) {
      Serial.println("WARNING: Unable to update calibration!");
    } else {
      Serial.println("WARNING: Unable to set calibration!");
    }
  }

  Serial.println("----------- end setCalibration -----------");
}

bool sturdyHTTP(String url, int attempts) {
  int num_attempts = 0;
  int bytesRead = 0, totalBytesRead = 0;
  bool success = true;

  while (num_attempts < attempts) {
    // Use HTTPClient class to create HTTP/TCP connections
    HTTPClient http;

    Serial.print("[sturdyHTTP] Attempt ");
    Serial.print(num_attempts + 1);
    Serial.print("/");
    Serial.println(attempts);
#ifdef ENABLE_DEBUG_SERIAL
    // Print out the request we're going to send!
    Serial.println("[sturdyHTTP] >> " + url);
#endif

    // WiFi MAC address should be populated at this point!
    http.begin(url);
  
    // start connection and send HTTP header
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      Serial.println("[sturdyHTTP] ** Server responded");
      String payload = http.getString();
      totalBytesRead = payload.length();
  
      Serial.println("[sturdyHTTP] << " + payload);
      
      // SenseOK:#:#
      if (payload.indexOf("SenseOK") != -1) {
        setCalibration(payload, 1);
      }
      
      if (totalBytesRead == 0) {
        Serial.println("[sturdyHTTP] ERROR: Failed to read data from server!");
      }
      
      Serial.println();
      Serial.print("[sturdyHTTP] ** Closing connection (");
      Serial.print(totalBytesRead);
      Serial.println(" bytes read)");
      success = true;
    } else {
      Serial.printf("[sturdyHTTP] HTTP GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      success = false;
    }
  
    http.end();

    if (success) break;
    num_attempts++;
  }

  return success;
}

/* Submit the data to the MicSense server, given the time number,
 * number of total samples taken, number of high samples detected, and
 * number of mid samples detected.
 */
void submitSum(uint32_t curTime, long totalSampleCount, long highTotalCount, long midTotalCount) {
  int bytesRead = 0, totalBytesRead = 0;
  Serial.println("[submitSum] Submit code goes here!");

  // URL
  String mac_addr = String(mac[5], HEX) + String(mac[4], HEX) + String(mac[3], HEX) + String(mac[2], HEX) + String(mac[1], HEX) + String(mac[0], HEX);

  String final_url = "http://" + String(server) + "/datasubmit?id=" + mac_addr + "&d=" + String(now()) + "," + String(highTotalCount) + "," + String(midTotalCount) + "," + String(totalSampleCount - (highTotalCount + midTotalCount));

#ifdef ENABLE_DEBUG_SERIAL
  // Print out the request we're going to send!
  Serial.println("[submitSum] >> " + final_url);
#endif
  
  bool submitSuccess = sturdyHTTP(final_url, 5);

  if (!submitSuccess) {
    Serial.println("[submitSum] Failed to submit data!");
  } else {
    Serial.println();
    Serial.print("[submitSum] Current mid thresh: ");
    Serial.println(mid_thresh);
    Serial.print("[submitSum] Current high thresh: ");
    Serial.println(high_thresh);
  }
}

void submitSumOLD(uint32_t curTime, long totalSampleCount, long highTotalCount, long midTotalCount) {
  int bytesRead = 0, totalBytesRead = 0;
  Serial.println("Submit code goes here!");

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  
  // WiFi MAC address should be populated at this point!
  
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
#ifdef ENABLE_DEBUG_SERIAL
    // Print out the request we're going to send!
    Serial.print("GET /datasubmit?id=");
    Serial.print(mac[5],HEX);
    Serial.print(mac[4],HEX);
    Serial.print(mac[3],HEX);
    Serial.print(mac[2],HEX);
    Serial.print(mac[1],HEX);
    Serial.print(mac[0],HEX);
    Serial.print("&d=");
    Serial.print(now());
    Serial.print(",");
    Serial.print(highTotalCount);
    Serial.print(",");
    Serial.print(midTotalCount);
    Serial.print(",");
    Serial.print(totalSampleCount - (highTotalCount + midTotalCount));
    Serial.println(" HTTP/1.1");
    Serial.print("Host: ");
    Serial.println(server);
    Serial.println("Connection: close");
    Serial.println();
#endif
    
    // Make a HTTP request:
    client.print("GET /datasubmit?id=");
    client.print(mac[5],HEX);
    client.print(mac[4],HEX);
    client.print(mac[3],HEX);
    client.print(mac[2],HEX);
    client.print(mac[1],HEX);
    client.print(mac[0],HEX);
    client.print("&d=");
    client.print(now());
    client.print(",");
    client.print(highTotalCount);
    client.print(",");
    client.print(midTotalCount);
    client.print(",");
    client.print(totalSampleCount - (highTotalCount + midTotalCount));
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();

    Serial.println("all data sent");
    waitForClient(client);
    // Read all the lines of the reply from server and print them to Serial
    while(bytesRead = client.available()){
      totalBytesRead += bytesRead;
      String line = client.readStringUntil('\r');
      Serial.print(line);
      // SenseOK:#:#
      if (line.indexOf("SenseOK") != -1) {
        setCalibration(line, 1);
      }
    }

    if (totalBytesRead == 0) {
      Serial.println("ERROR: Failed to read data from server!");
    }

    Serial.println();
    Serial.print("Current mid thresh: ");
    Serial.println(mid_thresh);
    Serial.print("Current high thresh: ");
    Serial.println(high_thresh);
    
    Serial.println();
    Serial.print("closing connection (");
    Serial.print(totalBytesRead);
    Serial.println(" bytes read)");
  } else {
    Serial.println("ERROR: Connection failed!");
    Serial.print("WiFi.status() = ");
    Serial.println(WiFi.status());
  }
}

void getCalibration() {
  int bytesRead = 0, totalBytesRead = 0;
  Serial.println("[getCalibration] Calibration code goes here!");

  // URL
  String mac_addr = String(mac[5], HEX) + String(mac[4], HEX) + String(mac[3], HEX) + String(mac[2], HEX) + String(mac[1], HEX) + String(mac[0], HEX);

  String final_url = "http://" + String(server) + "/calibrate?id=" + mac_addr;

#ifdef ENABLE_DEBUG_SERIAL
  // Print out the request we're going to send!
  Serial.println("[getCalibration] >> " + final_url);
#endif

  // We use 30 here because our data is essentially junk if we don't
  // have a valid calibration.
  bool submitSuccess = sturdyHTTP(final_url, 30);

  if (!submitSuccess) {
    Serial.println("[getCalibration] Failed to calibrate!");
  } else {
    Serial.println();
    Serial.print("[getCalibration] Current mid thresh: ");
    Serial.println(mid_thresh);
    Serial.print("[getCalibration] Current high thresh: ");
    Serial.println(high_thresh);
  }
}

void getCalibrationOLD() {
  int bytesRead = 0, totalBytesRead = 0;
  bool calibrationSet = false;
  Serial.println("Calibration code goes here!");

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  
  WiFi.macAddress(mac);
  
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
#ifdef ENABLE_DEBUG_SERIAL
    // Print out the request we're going to send!
    Serial.print("GET /calibrate?id=");
    Serial.print(mac[5],HEX);
    Serial.print(mac[4],HEX);
    Serial.print(mac[3],HEX);
    Serial.print(mac[2],HEX);
    Serial.print(mac[1],HEX);
    Serial.print(mac[0],HEX);
    Serial.println(" HTTP/1.1");
    Serial.print("Host: ");
    Serial.println(server);
    Serial.println("Connection: close");
    Serial.println();
#endif
    
    // Make a HTTP request:
    client.print("GET /calibrate?id=");
    client.print(mac[5],HEX);
    client.print(mac[4],HEX);
    client.print(mac[3],HEX);
    client.print(mac[2],HEX);
    client.print(mac[1],HEX);
    client.print(mac[0],HEX);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
    client.println("Connection: close\r\n\r\n");

    Serial.println("all data sent");
    waitForClient(client);
    // Read all the lines of the reply from server and print them to Serial
    while(bytesRead = client.available()){
      totalBytesRead += bytesRead;
      String line = client.readStringUntil('\r');
      Serial.print(line);
      // SenseOK:#:#
      if (line.indexOf("SenseOK") != -1) {
        setCalibration(line, 1);
      }
    }

    if (totalBytesRead == 0) {
      Serial.println("ERROR: Failed to read data from server!");
    }
    
    Serial.println();
    Serial.print("Current mid thresh: ");
    Serial.println(mid_thresh);
    Serial.print("Current high thresh: ");
    Serial.println(high_thresh);
    
    Serial.println();
    Serial.println("closing connection");
  } else {
    Serial.println("ERROR: Connection failed!");
  }
}
