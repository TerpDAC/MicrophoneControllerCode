#include "Time.h"
#include "TimeHandler.h"
#include "WiFiConn.h"

/* 
 * Active mode  = 1
 * Passive mode = 0
 */
int mode = 0;
int tmpValue = 0;
int histValues[100000];
stimer_t *mtimer;

/* Microphone settings */
#define THRESH_DIV 
#define HIGH_THRESH 
int micValue = 0;


#define ANALOG_PIN A0

void collectSum(int lengthSecs, int freq) {
  tmpValue = analogRead(ANALOG_PIN);
}

void sampleSound() {
  micValue = analogRead(A0);

  // Total is 1024, median is 512 = neutral value
  micValue = abs(micValue - 512);

  Serial.print("Analog adjusted: ");
  Serial.println(micValue);
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  initTime();
  Serial.println("Blocking until NTP time is fetched...");
  blockUntilTimeFetched();
  Serial.println("NTP time is fetched!");
  timerCreate(&mtimer);
}

void loop() {
  attemptToPrintTime();
  Serial.println(timerGetCurElapsed(&mtimer));
  sampleSound();

  if (timerGetCurElapsed(&mtimer) == 25) {
    if (getWiFiEnabled()) {
      Serial.println("Disabling WiFi...");
      wifiDisable();
      //wifiTest();
    } else {
      Serial.println("Enabling WiFi...");
      wifiEnable();
      //wifiTest();
    }
    timerReset(&mtimer);
  }
  
  delay(1000);
}

