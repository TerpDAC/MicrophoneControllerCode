#ifndef _CONFIG_H
#define _CONFIG_H

/* Microphone settings */

/* Define the minimum low percentage needed to trigger passive mode. */
#define LOW_MIN_PERCENTAGE 75

/* Define thresholds for medium/high modes */
#define MID_THRESH 72
#define HIGH_THRESH 80

/* Define time to gather as many samples as possible */
#define COLLECT_MIN 4
//#define COLLECT_SEC (COLLECT_MIN * 60)
//#define COLLECT_SEC 10
#define COLLECT_SEC 5

/* Define active mode sampling per hour */
/* Example: specifying 4 will mean sampling ever 15 minutes */
#define ACTIVE_HOUR_SAMPLING 4
#define ACTIVE_HOUR_SAMPLING_WAIT_MIN (60 / ACTIVE_HOUR_SAMPLING)

/* Microphone analog source pin */
#define ANALOG_PIN A0

#endif
