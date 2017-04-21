#ifndef _CONFIG_H
#define _CONFIG_H

/* Microphone settings */

/* Define the DC bias of the measurements */
#define DC_BIAS 512

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

/* Enable debugging?
 * Note that this does not disable all serial messages, but it does
 * make operations much faster because of less output.
 */
#define ENABLE_DEBUG_SERIAL

#endif
