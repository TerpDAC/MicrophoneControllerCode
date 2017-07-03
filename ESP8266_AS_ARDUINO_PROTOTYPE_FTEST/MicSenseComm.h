/**
 * @file   MicSenseComm.h
 * @brief  MicSense data uploading functions (header)
 *
 * Provides functions related to uploading the MicSense data, as well
 * as fetching calibration from the MicSense server. It also includes
 * private helper functions for (somewhat) reliable HTTP handling.
 * 
 * This is the header file for these functions. See source for actual
 * documentation.
 * 
 * @author MicSense Team / Terrapin Development and Consulting
 * @date   May 2017
 */
/*====================================================================*/
#ifndef _MICSENSE_COMM_H
#define _MICSENSE_COMM_H

#include "config.h"

void submitSum(uint32_t curTime, long totalSampleCount, long highTotalCount, long midTotalCount);
void getCalibration();

/* Specify whether to preload the server configuration or not.
 * Server configuration defined here will be loaded into EEPROM
 * memory on startup, then stop to allow the provisioner
 * to reset and disable the preload option.
 */
//#define PRELOAD_SERVER_CONFIG 1

/**
 * Specify your server configuration here to preload. PRELOAD_SERVER_CONFIG
 * must be defined in order for this to take any effect.
 */
#if defined(PRELOAD_SERVER_CONFIG) || defined(PRELOAD_ALL_CONFIG)

/**
 * Server to send data to. Can either be a TLD (e.g. server.com)
 * or an IP address. Note that this is just the server name/address - 
 * it is NOT a URL field!
 */
#define PRELOAD_MICSENSE_SERVER   "micsense_server.com"

/**
 * Whether to use HTTPS or not. Note that HTTPS may fail due to
 * memory constraints - if it fails, disable it!
 * 1 to enable, 0 to disable.
 */
#define PRELOAD_USE_HTTPS         0

/**
 * HTTPS SSL fingerprint. This is required if you want to use HTTPS,
 * since we want to verify that the server is who they say they are.
 * On Linux/Mac, you can get this fingerprint via this command:
 * 
 * echo | openssl s_client -connect micsense_server.com:443 |& openssl x509 -fingerprint -noout
 * 
 * (Replace micsense_server.com with the actual server name/address.)
 * 
 * Note that this command gives you a SHA1 hash. In the future, when
 * ESP8266's Arduino API supports SHA256 or better hashing, you may
 * use that here - for those longer hashes, make sure that you remove
 * any colons or spaces within the fingerprint. Colons and spaces are fine
 * for SHA1, but there isn't enough space for them in SHA256 and better.
 */
#define PRELOAD_HTTPS_FINGERPRINT ""

#endif

/* Specify the server that the MicSense should contact for data
 * and calibration.
 */
extern char micsense_server[65];

/* Specify whether this should use HTTPS or not.
 */
extern int  https_enabled;

/* Specify a HTTPS fingerprint to use, if HTTPS is enabled.
 */
extern char https_fingerprint[129];

#endif
