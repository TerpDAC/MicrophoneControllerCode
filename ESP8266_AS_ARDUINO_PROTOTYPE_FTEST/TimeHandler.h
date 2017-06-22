#ifndef _TIME_HANDLER_H
#define _TIME_HANDLER_H
#include <TimeLib.h>
#include <WiFiUdp.h>

time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);
void attemptToPrintTime();
void initTime();
void blockUntilTimeFetched();

typedef struct  { 
  uint32_t start;
  uint32_t stop;
} stimer_t;

void timerCreate(stimer_t **timer);
void timerDestroy(stimer_t **timer);
uint32_t timerGetCurElapsed(stimer_t **timer);
uint32_t timerStop(stimer_t **timer);
uint32_t timerGetFinalElapsed(stimer_t **timer);
uint32_t timerReset(stimer_t **timer);
#endif
