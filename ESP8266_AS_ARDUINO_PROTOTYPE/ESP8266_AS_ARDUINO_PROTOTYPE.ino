#include "Time.h"
#include "TimeHandler.h"
#include "WiFiConn.h"
#include "config.h"

/* 
 * Active mode  = 1
 * Passive mode = 0
 */
int currentMode = 0;
stimer_t *mtimer;

int micValue = 0;
int historyTable[4];

void adjustMode() {
  // historyTable stores percentage of samples that
  // did NOT meet thresholds... if it's at least 75%,
  // then trigger passive mode. Otherwise, if any of
  // the values are less than 75%, then trigger active
  // mode.
  if ((historyTable[0] < LOW_MIN_PERCENTAGE) || \
    (historyTable[1] < LOW_MIN_PERCENTAGE) || \
    (historyTable[2] < LOW_MIN_PERCENTAGE) || \
    (historyTable[3] < LOW_MIN_PERCENTAGE)) {
    currentMode = 1;
  } else {
    currentMode = 0;
  }
}

// Collect sound for predefined amount of time, and classify the sounds.
void collectSum() {
  long totalSampleCount = 0;
  long highTotalCount = 0;
  long midTotalCount = 0;
  
  stimer_t *sumtimer;

  Serial.println("Collecting data...");
  
  timerCreate(&sumtimer);

  while (timerGetCurElapsed(&sumtimer) < COLLECT_SEC) {
    micValue = analogRead(ANALOG_PIN);
    micValue = abs(micValue - 512);
    
    totalSampleCount++;

    if (micValue >= HIGH_THRESH) {
      highTotalCount++;
    } else if (micValue >= MID_THRESH) {
      midTotalCount++;
    }

    // Feed WDT so it doesn't think we died!
    ESP.wdtFeed();
  }

  Serial.print("Total samples taken: ");
  Serial.println(totalSampleCount);
  Serial.print("High samples: ");
  Serial.println(highTotalCount);
  Serial.print("Medium samples: ");
  Serial.println(midTotalCount);

  Serial.print("Current mode: ");
  Serial.println(currentMode);
  
  adjustMode();

  Serial.print("Mode after adjustment, if any: ");
  Serial.println(currentMode);

  submitSum(sumtimer->start, totalSampleCount, highTotalCount, midTotalCount);
}

// Active mode
void execActiveMode() {
  // Active mode
  int timesRun = 0;

  Serial.println("Running in active mode...");

  /*
  while (timesRun < ACTIVE_HOUR_SAMPLING) {
    if ((minute() % ACTIVE_HOUR_SAMPLING_WAIT_MIN) == 0) {
      collectSum();
      timesRun++;
      //espSleep(100);
      espSleep(2);
    } else {
      //espSleep((minute() % ACTIVE_HOUR_SAMPLING_WAIT_MIN) * 60);
      espSleep(2);
    }
  }
  */

  collectSum();
}

// Passive mode
void execPassiveMode() {
  // Passive mode
  int timesRun = 0;
  uint32_t disableSecs = 0;

  Serial.println("Running in passive mode...");
  
  while (timesRun == 0) {
    if (minute() >= 45) {
      collectSum();
      timesRun++;
    } else {
      if ((45 - minute()) > 1) {
        // Leave one minute off, just in case!
        disableSecs = abs(44 - minute()) * 60;
        Serial.println("Sending disableSecs:");
        Serial.println(disableSecs);
        //espSleep(disableSecs);
        espSleep(5);
      } else {
        // Sleep very briefly... ~30s.
        //espSleep(30);
        espSleep(1);
      }
    }
  }
}

/* 
 * Active mode  = 1
 * Passive mode = 0
 */
void execMode() {
  Serial.println("Executing mode!");
  if (currentMode == 1) {
    // Active mode
    execActiveMode();
  } else {
    // Passive mode
    //execPassiveMode();
    execActiveMode();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 now in setup()!");
  Serial.println("Reset/startup reason:");
  Serial.println(ESP.getResetReason());
  connectToWiFi();
  initTime();
  Serial.println("Blocking until NTP time is fetched...");
  blockUntilTimeFetched();
  Serial.println("NTP time is fetched!");
  timerCreate(&mtimer);
}

void loop() {
  //collectSum();

  execMode();
  
  /*attemptToPrintTime();
  Serial.println(timerGetCurElapsed(&mtimer));
  //sampleSound();

  if (timerGetCurElapsed(&mtimer) == 30) {
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
  
  delay(1000);*/
}

