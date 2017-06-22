/**
 * @file   MicSenseComm.h
 * @brief  MicSense data uploading functions
 *
 * Provides functions related to uploading the MicSense data, as well
 * as fetching calibration from the MicSense server. It also includes
 * private helper functions for (somewhat) reliable HTTP handling.
 * 
 * @author MicSense Team / Terrapin Development and Consulting
 * @date   May 2017
 */

#include <ESP8266HTTPClient.h>
#include "MicSenseComm.h"
#include "config.h"
#include "util.h"
#include "WiFiConn.h"
#include "Time.h"

/*====================================================================*/

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

  String final_url = "http://" + MICSENSE_SERVER + "/datasubmit?id=" + mac_addr + "&d=" + String(now()) + "," + String(highTotalCount) + "," + String(midTotalCount) + "," + String(totalSampleCount - (highTotalCount + midTotalCount));

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

void getCalibration() {
  int bytesRead = 0, totalBytesRead = 0;
  Serial.println("[getCalibration] Calibration code goes here!");

  // URL
  String mac_addr = String(mac[5], HEX) + String(mac[4], HEX) + String(mac[3], HEX) + String(mac[2], HEX) + String(mac[1], HEX) + String(mac[0], HEX);

  String final_url = "http://" + MICSENSE_SERVER + "/calibrate?id=" + mac_addr;

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
