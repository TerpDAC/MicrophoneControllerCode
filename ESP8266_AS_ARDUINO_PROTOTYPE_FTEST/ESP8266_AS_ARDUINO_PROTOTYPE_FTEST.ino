// Calibration feature TODO:
//   - make it save the calibration in memory, for use if the WiFi is not available.
//   - separate functionality to different file
// WiFi feature TODO:
//   - delay on boot with indication from LEDs - blinking RED, solid BLUE
//   - on GPIO0 press, start AP
//   - 

#include "util.h"
#include "Time.h"
#include "TimeHandler.h"
#include "WiFiConn.h"
#include "MicSenseComm.h"
#include "config.h"
#include "led.h"

// Required for MAC change only
// When MAC change is removed, remove this as well.
extern "C" {
#include "user_interface.h"
}

int micValue = 0;

// Thresholds
int mid_thresh = 500;
int high_thresh = 600;

/*
void initVariant() {
  // 18:fe:34:01:59:ab
  //uint8_t mac[] = {0x18, 0xfe, 0x34, 0x01, 0x59, 0xab};
  uint8_t mac[] = {0xab, 0x59, 0x01, 0x34, 0xfe, 0x18};
  wifi_set_macaddr(STATION_IF, &mac[0]);
}
*/

// Collect sound for predefined amount of time, and classify the sounds.
void collectSum() {
  // Initialize counts.
  long totalSampleCount = 0;
  long highTotalCount = 0;
  long midTotalCount = 0;

  // Create timer.
  stimer_t *sumtimer;

  Serial.println("Collecting data...");
  Serial.print(" - Current mid thresh: ");
  Serial.println(mid_thresh);
  Serial.print(" - Current high thresh: ");
  Serial.println(high_thresh);

  // Initialize timer and start counting.
  timerCreate(&sumtimer);

  // Sanity check... did the timer actually get created?
  if (sumtimer == NULL) {
    Serial.println("[collectSum] Unable to create new timer - malloc failed in timerCreate");
    Serial.println("[collectSum] Due to critical error, MicSense will now restart.");
    ESP.restart();
    return;
  }

  // Fetch the elapsed time, and make sure it is less than COLLECT_SEC.
  // (from config.h)
  while (timerGetCurElapsed(&sumtimer) < COLLECT_SEC) {
    micValue = analogRead(ANALOG_PIN);
    micValue = abs(micValue - DC_BIAS);

    // Increment sample count
    totalSampleCount++;
    
    // Does the value meet the high threshold?
    // If so, increment the high total.
    if (micValue >= high_thresh) {
      highTotalCount++;
    } else if (micValue >= mid_thresh) {
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

  // Clean up
  timerDestroy(&sumtimer);
}

void setup() {
  // Setup USB serial!
  Serial.begin(115200);

  Serial.setDebugOutput(true);
  
  Serial.println("ESP8266 now in setup()!");
  Serial.println("Reset/startup reason:");
  Serial.println(ESP.getResetReason());

  // Set up LED system
  //Serial.println("Initializing LED subsystem...");
  initLED();
  //setLED(1);

  setRedLED(1);

  // Connect to WiFi
  connectToWiFi();
  
  // Start the timing mechanism (+NTP)
  initTime();
  
  Serial.println("Blocking until NTP time is fetched...");
  blockUntilTimeFetched();
  Serial.println("NTP time is fetched!");

  // Set LED off
  setRedLED(0);
  
  Serial.println("Attempting to calibrate sensors from the server...");
  getCalibration();
  Serial.println("Setup complete!");
}

void loop() {
  collectSum();
}

