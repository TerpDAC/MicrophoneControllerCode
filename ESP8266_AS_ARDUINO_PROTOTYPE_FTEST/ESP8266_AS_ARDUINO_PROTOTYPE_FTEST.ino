#include "Time.h"
#include "TimeHandler.h"
#include "WiFiConn.h"
#include "config.h"

stimer_t *mtimer;

int micValue = 0;

// Thresholds
int mid_thresh = 100;
int high_thresh = 200;

// Collect sound for predefined amount of time, and classify the sounds.
void collectSum() {
  // Initialize counts.
  long totalSampleCount = 0;
  long highTotalCount = 0;
  long midTotalCount = 0;

  // Create timer.
  stimer_t *sumtimer;

  Serial.println("Collecting data...");

  // Initialize timer and start counting.
  timerCreate(&sumtimer);

  // Fetch the elapsed time, and make sure it is less than COLLECT_SEC.
  // (from config.h)
  while (timerGetCurElapsed(&sumtimer) < COLLECT_SEC) {
    micValue = analogRead(ANALOG_PIN);
    micValue = abs(micValue - DC_BIAS);

    // Increment sample count
    totalSampleCount++;
    
    // Does the value meet the high threshold?
    // If so, increment the high total.
    if (micValue >= HIGH_THRESH) {
      highTotalCount++;
    } else if (micValue >= MID_THRESH) {
      // Does the value meet the medium threshold?
      // If so, increment the medium total.
      midTotalCount++;
    }
    // Otherwise, don't do anything. We can calculate low from
    // doing totalSampleCount - highTotalCount - midTotalCount!
    
    // Feed watchdog timer (WDT) so it doesn't think we died!
    ESP.wdtFeed();
  }

  // Print statistics of collection to serial for debugging
  Serial.print("Total samples taken: ");
  Serial.println(totalSampleCount);
  Serial.print("High samples: ");
  Serial.println(highTotalCount);
  Serial.print("Medium samples: ");
  Serial.println(midTotalCount);

  // Submit sums!
  // Arguments: start time, total samples, high samples, medium samples
  submitSum(sumtimer->start, totalSampleCount, highTotalCount, midTotalCount);
}

void setup() {
  // Setup USB serial!
  Serial.begin(115200);
  
  Serial.println("ESP8266 now in setup()!");
  Serial.println("Reset/startup reason:");
  Serial.println(ESP.getResetReason());
  
  connectToWiFi();
  
  // Start the timing mechanism (+NTP)
  initTime();
  
  Serial.println("Blocking until NTP time is fetched...");
  blockUntilTimeFetched();
  Serial.println("NTP time is fetched!");
  
  timerCreate(&mtimer);
  
  Serial.println("Attempting to calibrate sensors from the server...");
  getCalibration();
  Serial.println("Setup complete!");
}

void loop() {
  collectSum();
}

