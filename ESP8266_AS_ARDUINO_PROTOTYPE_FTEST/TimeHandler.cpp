/*
 * Time_NTP.pde
 * Example showing time sync to NTP time source
 *
 * This sketch uses the ESP8266WiFi library
 */
 
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "TimeHandler.h"
#include "WiFiConn.h"
#include "led.h"
#include "util.h"

// NTP Servers:
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov

//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
const int timeZone = 0;  // GMT/UTC

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

/* Enable WiFi if necessary, then fetch time.
 * This function is used for the time handler and is called at an interval.
 */
time_t getNtpTimeWiFiKick() {
  if (!getWiFiEnabled()) {
    wifiEnable();
  }
  return getNtpTime();
}

/* Initialize the time keeper. */
void initTime() 
{ 
  SerialPrintStrLn("Starting UDP");
  Udp.begin(localPort);
  SerialPrintStr("Local port: ");
  Serial.println(Udp.localPort());
  SerialPrintStrLn("waiting for sync");
  setSyncProvider(getNtpTimeWiFiKick);
  // Sync the time every 10 seconds!
  setSyncInterval(10);
}

time_t prevDisplay = 0; // when the digital clock was displayed

/* Block execution until the NTP time has been fetched. */
void blockUntilTimeFetched() {
  int count = 0;
  int ledEnabled = 0;
  while (timeStatus() == timeNotSet) {
    SerialPrintStrLn("Waiting...");
    delay(1000);
    count++;
    if (count == 5) getNtpTimeWiFiKick();
    if (ledEnabled) ledEnabled = 0; else ledEnabled = 1;
    setRedLED(ledEnabled);
  }
}

/* Print the time if there's any change. */
void attemptToPrintTime()
{  
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();  
    }
  }
}

/* Print the current time to serial. */
void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  SerialPrintStr(" ");
  Serial.print(day());
  SerialPrintStr(".");
  Serial.print(month());
  SerialPrintStr(".");
  Serial.print(year());
  Serial.println();
}

/* Create a new timer, given an empty timer argument.
 * 
 * Do NOT call this function multiple times on the same timer variable!
 * 
 * We strongly do not recommend using this function. This function requires
 * malloc, and malloc is generally not a good idea on embedded platforms.
 * If at all possible, stick to locally defined variables (in your function
 * stack).
 */
void timerCreate(stimer_t **timer) {
  blockUntilTimeFetched();
  stimer_t *new_timer;
  new_timer = (stimer_t *) malloc(sizeof(timer_t));

  // Sanity check
  if (new_timer == NULL) {
    // Uh oh, this can't be good...
    SerialPrintStrLn("[timerCreate] Unable to create new timer - malloc failed");
    *timer = NULL;
    return;
  }
  
  new_timer->start = now();
  *timer = new_timer;
}

/* Initialize a new timer, given a new timer argument.
 * 
 * Do NOT call this function multiple times on the same timer variable!
 */
void timerInit(stimer_t *timer) {
  blockUntilTimeFetched();
  timer->start = now();
}

/* Destroy an existing timer, given a timer created with timerCreate.
 * 
 * You must call this function before a timer variable created with
 * timerCreate goes out of scope. Otherwise, you will leak memory and
 * things will crash!
 * 
 * Do NOT call this function multiple times on the same timer variable,
 * and do NOT call this function on a timer variable not created with
 * timerCreate!
 * 
 * We strongly do not recommend using this function. This function implies
 * malloc, and malloc is generally not a good idea on embedded platforms.
 * If at all possible, stick to locally defined variables (in your function
 * stack).
 */
void timerDestroy(stimer_t **timer) {
  if (*timer != NULL) {
    free(*timer);
  }
}

/* Reset the specified timer. */
uint32_t timerReset(stimer_t *timer) {
  uint32_t elapsed = now() - timer->start;
  timer->start = now();
  return elapsed;
}

/* Get the current elapsed time (in seconds) for the specified timer. */
uint32_t timerGetCurElapsed(stimer_t *timer) {
  return (now() - timer->start);
}

/* Stop the specified timer and return the timer's elapsed time. */
uint32_t timerStop(stimer_t *timer) {
  timer->stop = now();
  return (timer->stop - timer->start);
}

/* Return the timer's elapsed time for the specified timer.
 * Note that timer should have been stopped in order for this to
 * return anything meaningful.
 */
uint32_t timerGetFinalElapsed(stimer_t *timer) {
  return (timer->stop - timer->start);
}

/* Print a number formatted to digital clock type.
 * (e.g. 8 => 08, 10 => 10)
 */
void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  SerialPrintStr(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

/* Return the NTP synchronized time */
time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  SerialPrintStrLn("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      SerialPrintStrLn("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  SerialPrintStrLn("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// Send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
