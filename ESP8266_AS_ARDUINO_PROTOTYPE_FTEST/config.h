#ifndef _CONFIG_H
#define _CONFIG_H

extern "C" {
#include <stdint.h>
}

/* Microphone settings */

/* Define the DC bias of the measurements */
#define DC_BIAS 0

/* Define thresholds for medium/high modes */
//#define MID_THRESH 57
//#define HIGH_THRESH 64

// Thresholds are now dynamic, and are located within the INO source.
extern int mid_thresh;
extern int high_thresh;

/* Define time to gather as many samples as possible */
#define COLLECT_SEC 10

/* Microphone analog source pin */
#define ANALOG_PIN A0

/* Maximum HTTP response size
 * Should be set to something very low to avoid memory alloc issues
 * Default is 25 (SenseOK:NNNN:NNNN\n = 18)
 */
#define MAX_HTTP_RESP_SIZE 25

/* Enable debugging?
 * Note that this does not disable all serial messages, but it does
 * make operations much faster because of less output.
 */
#define ENABLE_DEBUG_SERIAL

/* Enable verbose debugging?
 * Note that this does not disable all serial messages, but it does
 * make operations much faster because of less output.
 */
//#define ENABLE_DEBUG_SERIAL_VERBOSE

/* Specify whether to preload all of the configuration or not.
 * WiFi and server configuration will be loaded into EEPROM
 * memory on startup, then stop to allow the provisioner
 * to reset and disable the preload option.
 * 
 * WiFi preload configuration can be found in WiFiConfig.h, and
 * server preload configuration can be found in MicSenseComm.h.
 */
//#define PRELOAD_ALL_CONFIG 1

/* SPIFFS Definitions */
void spiffsInit();

/* Force SPIFFS to be formatted on startup. This should be enabled if
 * you suspect any issues with the SPIFFS. Note that once enabled,
 * you need to let it run once, and then uncomment this line and resend
 * once the formatting completes.
 */
//#define FORCE_SPIFFS_FORMAT 1

/* EEPROM Definitions */
void eepromInit();

/* Configuration Definitions */
void getSalt(uint8_t *salt);
void loadLocalThresholds();
void saveLocalThresholds();
void loadLocalWiFiCredentials();
void saveLocalWiFiCredentials(const char ssid[33], const char psk[65], const char config_pass[64]);
void loadLocalServerCredentials();
void saveLocalServerCredentials(const char micsense_server[65], const int use_https, const char https_fingerprint[129]);
void loadAllConfiguration();

/* Specify the CRC32 size. This will always be 4 bytes. */
#define EEPROM_DATA_CHUNK_CRC_SIZE 4

/* Individual salt size - salts for individual configuration pieces. */
#define INDV_SALT_SIZE 64

/* Raw key size - the raw key size for each individual configuration
 * pieces.
 */
#define RAW_KEY_SIZE 1024

/* General salt size - the size of the salt for the key scrambling
 * function, designed to prevent simple code peeking. (Does NOT prevent
 * any experienced people from figuring the data out, though!)
 */
#define SALT_SIZE 128

/* Helper macro to add the CRC data chunk size into the
 * data size calculation.
 */
#define ADD_CRC(size) (size + EEPROM_DATA_CHUNK_CRC_SIZE)

/* Helper macro to subtract the CRC data chunk size into the
 * data size calculation.
 */
#define SUB_CRC(size) (size - EEPROM_DATA_CHUNK_CRC_SIZE)

/* Helper macro to help round up to a certain byte amount.
 */
#define BYTES_ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))

/* Helper macro to align bytes to 8 byte chunks, for EEPROM storage.
 */
#define EEPROM_BOUNDS(size) BYTES_ROUND_UP(size, 8)

/* Specify the encrypted extra data size. This will always be
 * 64 bytes (salt) + 16 bytes (AES GCM tag) + 12 bytes (AES IV) = 92.
 */
#define EEPROM_DATA_CHUNK_ENC_SIZE 92

/* Helper macro to add the encrypted data chunk size into the
 * data size calculation. Will automatically round up the size
 * into chunks of 16.
 */
#define ADD_ENC(size) (EEPROM_DATA_CHUNK_ENC_SIZE + BYTES_ROUND_UP(size, 16))

/* Helper macro to subtract the encrypted data chunk size from the
 * data size calculation.
 */
#define SUB_ENC(size) (size - EEPROM_DATA_CHUNK_ENC_SIZE)

/* Specify the EEPROM offset to read from for mid_thresh data
 * chunk. This offset includes the CRC.
 */
#define EEPROM_MID_THRESH_OFFSET 0

/* Specify the mid_thresh data chunk total size, including the
 * CRC. Integer size is 4.
 */
#define EEPROM_MID_THRESH_SIZE ADD_CRC(4)

/* Specify the EEPROM offset to read from for high_thresh data
 * chunk. This offset includes the CRC.
 */
#define EEPROM_HIGH_THRESH_OFFSET EEPROM_BOUNDS(EEPROM_MID_THRESH_OFFSET + EEPROM_MID_THRESH_SIZE)

/* Specify the high_thresh data chunk total size, including the
 * CRC. Integer size is 4.
 */
#define EEPROM_HIGH_THRESH_SIZE ADD_CRC(4)

/* Specify the EEPROM offset to read from for WiFi SSID data
 * chunk. This offset includes the CRC.
 */
#define EEPROM_WIFI_SSID_OFFSET EEPROM_BOUNDS(EEPROM_HIGH_THRESH_OFFSET + EEPROM_HIGH_THRESH_SIZE)

/* Specify the WiFi SSID data chunk total size, including the
 * CRC. SSID size is 32, and this needs to be encrypted.
 */
#define EEPROM_WIFI_SSID_SIZE ADD_CRC(ADD_ENC(32))

/* Specify the EEPROM offset to read from for WiFi PSK data
 * chunk. This offset includes the CRC.
 */
#define EEPROM_WIFI_PSK_OFFSET EEPROM_BOUNDS(EEPROM_WIFI_SSID_OFFSET + EEPROM_WIFI_SSID_SIZE)

/* Specify the WiFi SSID data chunk total size, including the
 * CRC. PSK size is 64, and this needs to be encrypted.
 */
#define EEPROM_WIFI_PSK_SIZE ADD_CRC(ADD_ENC(64))

/* Specify the EEPROM offset to read from for MicSense server
 * address data chunk. This offset includes the CRC.
 */
#define EEPROM_MICSENSE_SERVER_OFFSET EEPROM_BOUNDS(EEPROM_WIFI_PSK_OFFSET + EEPROM_WIFI_PSK_SIZE)

/* Specify the MicSense server address data chunk total size,
 * including the CRC. Server address size is 64, and this needs
 * to be encrypted.
 */
#define EEPROM_MICSENSE_SERVER_SIZE ADD_CRC(ADD_ENC(64))

/* Specify the EEPROM offset to read from for HTTPS usage
 * configuration flag data chunk. This offset includes the CRC.
 */
#define EEPROM_USE_HTTPS_OFFSET EEPROM_BOUNDS(EEPROM_MICSENSE_SERVER_OFFSET + EEPROM_MICSENSE_SERVER_SIZE)

/* Specify the HTTPS usage configuration flag data chunk total
 * size, including the CRC.
 */
#define EEPROM_USE_HTTPS_SIZE ADD_CRC(1)

/* Specify the EEPROM offset to read from for HTTPS fingerprint
 * data chunk. This offset includes the CRC.
 */
#define EEPROM_HTTPS_FINGERPRINT_OFFSET EEPROM_BOUNDS(EEPROM_USE_HTTPS_OFFSET + EEPROM_USE_HTTPS_SIZE)

/* Specify the HTTPS fingerprint data chunk total size, including
 * the CRC. Max HTTPS fingerprint is 128, and this needs to be
 * encrypted.
 */
#define EEPROM_HTTPS_FINGERPRINT_SIZE ADD_CRC(ADD_ENC(128))

/* Specify the EEPROM offset to read from for configuration
 * password data chunk. This offset includes the CRC.
 */
#define EEPROM_CONFIG_PASS_OFFSET EEPROM_BOUNDS(EEPROM_HTTPS_FINGERPRINT_OFFSET + EEPROM_HTTPS_FINGERPRINT_SIZE)

/* Specify the configuration password data chunk total size,
 * including the CRC. Max password size is 64, and this needs
 * to be encrypted.
 */
#define EEPROM_CONFIG_PASS_SIZE ADD_CRC(ADD_ENC(64))

/* definition to expand macro then apply to pragma message */
/*
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)
#pragma message(VAR_NAME_VALUE(EEPROM_HTTPS_FINGERPRINT_OFFSET))
#pragma message(VAR_NAME_VALUE(EEPROM_HTTPS_FINGERPRINT_SIZE))
*/

#endif
