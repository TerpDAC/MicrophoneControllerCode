/**
 * @file   led.cpp
 * @brief  LED blinking functions
 *
 * Provides functions related to LED blinking - setup, teardown, and
 * LED mode setting. LED operation is provided via interrupts on this
 * device. For static LED modes (e.g. completely off or completely on,
 * no blinking), the timer interrupt is disabled.
 * 
 * @author MicSense Team / Terrapin Development and Consulting
 * @date   May 2017
 */

/*====================================================================*/
#include <Arduino.h>
#include "led.h"

/**
 * Global variable defining currently LED state set by user.
 */
volatile int ledEnabled = 0;

/**
 * Global variable defining internal LED state.
 * 
 * Used for blinking LED. It is compared with ledEnabled to determine
 * when to toggle the LED on or off.
 */
volatile int ledState = 0;

/**
 * Initialize the LED subsystem.
 * 
 * Set up interrupts to make LED blinking work. This should only be
 * called once.
 * 
 * @see initLEDTimer()
 */
void initLED() {
  pinMode(LED_PIN, OUTPUT);
  //initLEDTimer();
  //startLEDTimer();
}

/**
 * Initialize the LED interrupt.
 * 
 * Set up the LED interrupt timer, and attach the corresponding
 * interrupt service request (ISR) function to said timer. Note that
 * this does NOT start the timer, only initialize it.
 * 
 * You should not call this directly - use initLED() instead.
 * 
 * @see initLED()
 */
void initLEDTimer() {
  ESP.wdtFeed();
  //ESP.wdtEnable(5000);
  
  // Disable interrupts
  noInterrupts();

  // Initialize timer
  timer1_isr_init();
  timer1_write(ESP.getCycleCount() + LED_TIMER_DELAY);
  timer1_attachInterrupt(ledISR);

  // Re-enable interrupts
  interrupts();

  ESP.wdtFeed();
}

/**
 * Start the LED interrupt timer.
 * 
 * Start up the LED interrupt timer. Once started, the interrupt will
 * call the function specified in initLEDTimer().
 * 
 * This assumes that initLEDTimer() has already been called. If it has
 * not been called, this function will not do anything.
 * 
 * You should not call this directly - setLED() will handle starting
 * and stopping the LED interrupt timer automatically depending on the
 * mode selected.
 * 
 * @see initLEDTimer()
 * @see setLED()
 */
void startLEDTimer() {
  ESP.wdtFeed();
  //ESP.wdtEnable(5000);
  
  // Disable interrupts
  noInterrupts();

  // Enable/start timer
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);

  // Re-enable interrupts
  interrupts();

  ESP.wdtFeed();
}

/**
 * Stop the LED interrupt timer.
 * 
 * Halt the LED interrupt timer.
 * 
 * You should not call this directly - setLED() will handle starting
 * and stopping the LED interrupt timer automatically depending on the
 * mode selected.
 * 
 * @see setLED()
 */
void stopLEDTimer() {
  timer1_disable();
}

/**
 * Set the LED on/off/blinking state.
 * 
 * Disable, enable, or blink the LED.
 * 
 * Note that changes will only be reflected in the hardware if
 * initLED() has been called.
 * 
 * @param [in] enabled  State to set the LED.
 *                      0: disabled
 *                      1: enabled
 *                      2: blinking
 *                      3+: slower blinking
 * @see initLED()
 */
void setLED(int enabled) {
  asm volatile ("" : : : "memory");
  ledEnabled = enabled;
  if (ledEnabled > 1) {
    startLEDTimer();
  } else {
    ledISR();
    stopLEDTimer();
  }
}

/*====================================================================*/
/**
 * Interrupt service request handler for LED subsystem.
 * 
 * Handles turning the LED on/off, as well as any logic needed to
 * control the blinking of the LED.
 * 
 * This is called by the timer interrupt at an interval, subject to
 * other interrupt handling by the chip. This is setup from the
 * initLEDTimer() call.
 * 
 * This function can be called directly as needed, but it is best to
 * leave the handling to setLED().
 * 
 * @see initLEDTimer()
 * @see setLED()
 */
void ICACHE_RAM_ATTR ledISR(void) {
  // If ledEnabled is set, then
  //     ledEnabled > 1, then
  //       if ledState matches ledEnabled, write 1
  //       else write 0
  //     else (if ledEnabled is just 1), write 1
  // else write 0
  digitalWrite(LED_PIN, ledEnabled ? ( (ledEnabled > 1) ? ( (ledState == ledEnabled) ? LED_ON : LED_OFF ) : LED_ON ) : LED_OFF);

  // If ledEnabled > 1, then
  //   if ledState >= ledEnabled, then reset to 0
  //   else keep incrementing
  // else just write zero
  ledState = (ledEnabled > 1) ? ( (ledState >= ledEnabled) ? 0 : (ledState + 1) ) : 0;

  // Update timer
  timer1_write(ESP.getCycleCount() + LED_TIMER_DELAY);

  /*
  Serial.print("LED enabled: ");
  Serial.print(ledEnabled);
  Serial.print(" | LED state: ");
  Serial.print(ledState);
  Serial.print(" | Final written state: ");
  Serial.println(ledEnabled ? ( (ledEnabled > 1) ? ( (ledState == ledEnabled) ? 1 : 0 ) : 1 ) : 0);
  */
}

void setRedLED(int enabled) {
  digitalWrite(LED_PIN, (enabled) ? LED_ON : LED_OFF);
}

