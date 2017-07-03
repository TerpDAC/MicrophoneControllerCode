/**
 * @file   WiFiConn.h
 * @brief  WiFi connection functions (header)
 *
 * Provides functions related to WiFi - setup, teardown, and
 * related control functions. WiFi is provided via interrupts on this
 * device. (This is hidden within the ESP8266 WiFi library.)
 * 
 * This is the header file for these functions. See source for actual
 * documentation.
 * 
 * @author MicSense Team / Terrapin Development and Consulting
 * @date   May 2017
 */

#ifndef _WIFI_CONN_H
#define _WIFI_CONN_H
void connectToWiFi();
int getWiFiEnabled();
void wifiDisable();
void wifiEnable();
void wifiTest();
void espDeepSleep(uint32_t sec);
void espSleep(uint32_t sec);

// Shared variables
extern uint8_t mac[6];
extern char ssid[33];
extern char password[65];
#endif
