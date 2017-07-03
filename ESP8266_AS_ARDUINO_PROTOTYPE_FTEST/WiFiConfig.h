#ifndef _WIFI_CONFIG_H
#define _WIFI_CONFIG_H

#include "config.h"

/* Specify whether to preload the WiFi credentials or not.
 * WiFi credentials defined here will be loaded into EEPROM
 * memory on startup, then stop to allow the provisioner
 * to reset and disable the preload option.
 */
//#define PRELOAD_SSID_PSK 1

/**
 * Specify your WiFi SSID and password here to preload. PRELOAD_SSID_PSK
 * must be defined in order for this to take any effect.
 */
#if defined(PRELOAD_SSID_PSK) || defined(PRELOAD_ALL_CONFIG)

/**
 * WiFi SSID
 */
#define PRELOAD_SSID "YOUR_SSID"

/**
 * WiFi password (or PSK)
 */
#define PRELOAD_PASS "YOUR_PASS"
#endif

#endif
