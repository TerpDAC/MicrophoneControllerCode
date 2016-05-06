#ifndef _CONFIG_H
#define _CONFIG_H

/* Microphone settings */

/* Define the DC bias of the measurements */
#define DC_BIAS 512

/* Define thresholds for medium/high modes */
#define MID_THRESH 57
#define HIGH_THRESH 64

/* Define time to gather as many samples as possible */
#define COLLECT_SEC 10

/* Microphone analog source pin */
#define ANALOG_PIN A0

#endif
