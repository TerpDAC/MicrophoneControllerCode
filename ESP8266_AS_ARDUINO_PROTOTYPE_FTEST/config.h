#ifndef _CONFIG_H
#define _CONFIG_H

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

/* Specify the server that the MicSense should contact for data
 * and calibration.
 */
#define MICSENSE_SERVER "terpdac.pythonanywhere.com"

/* EEPROM Definitions */
void eepromInit();
void loadLocalThresholds();
void saveLocalThresholds();

/* Specify the CRC32 size. This will always be 4 bytes. */
#define EEPROM_DATA_CHUNK_CRC_SIZE 4

/* Specify the EEPROM offset to read from for mid_thresh data
 * chunk. This offset includes the CRC.
 */
#define EEPROM_MID_THRESH_OFFSET 0

/* Specify the mid_thresh data chunk total size, including the
 * CRC.
 */
#define EEPROM_MID_THRESH_SIZE 8

/* Specify the EEPROM offset to read from for high_thresh data
 * chunk. This offset includes the CRC.
 */
#define EEPROM_HIGH_THRESH_OFFSET 8

/* Specify the high_thresh data chunk total size, including the
 * CRC.
 */
#define EEPROM_HIGH_THRESH_SIZE 8

/* Specify the EEPROM offset to read from for WiFi SSID data
 * chunk. This offset includes the CRC.
 */
#define EEPROM_WIFI_SSID_OFFSET 16

/* Specify the WiFi SSID data chunk total size, including the
 * CRC.
 */
#define EEPROM_WIFI_SSID_SIZE 37

/* Specify the EEPROM offset to read from for WiFi PSK data
 * chunk. This offset includes the CRC.
 */
#define EEPROM_WIFI_PSK_OFFSET 56

/* Specify the WiFi SSID data chunk total size, including the
 * CRC.
 */
#define EEPROM_WIFI_PSK_SIZE 69

/* Specify the EEPROM offset to read from for MicSense server
 * address data chunk. This offset includes the CRC.
 */
#define EEPROM_MICSENSE_SERVER_OFFSET 128

/* Specify the MicSense server address data chunk total size,
 * including the CRC.
 */
#define EEPROM_MICSENSE_SERVER_SIZE 69

/* Specify the EEPROM offset to read from for HTTPS usage
 * configuration flag data chunk. This offset includes the CRC.
 */
#define EEPROM_USE_HTTPS_OFFSET 200

/* Specify the HTTPS usage configuration flag data chunk total
 * size, including the CRC.
 */
#define EEPROM_USE_HTTPS_SIZE 5

/* Specify the EEPROM offset to read from for HTTPS fingerprint
 * data chunk. This offset includes the CRC.
 */
#define EEPROM_HTTPS_FINGERPRINT_OFFSET 200

/* Specify the HTTPS fingerprint data chunk total size, including
 * the CRC.
 */
#define EEPROM_HTTPS_FINGERPRINT_SIZE 133

#endif
