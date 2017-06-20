/**
 * @file   led.h
 * @brief  LED blinking functions (header)
 *
 * Provides functions related to LED blinking - setup, teardown, and
 * LED mode setting. LED operation is provided via interrupts on this
 * device. For static LED modes (e.g. completely off or completely on,
 * no blinking), the timer interrupt is disabled.
 * 
 * This is the header file for these functions. See source for actual
 * documentation.
 * 
 * @author MicSense Team / Terrapin Development and Consulting
 * @date   May 2017
 */
#ifndef _LED_H
#define _LED_H

#include <Arduino.h>

/**
 * Constant defining the LED pin used.
 */
#define LED_PIN 0

/**
 * Constant defining the timer delay interval used.
 */
#define LED_TIMER_DELAY 7000 //4166

/**
 * Constant defining the LED on state, relative to hardware.
 */
#define LED_ON LOW

/**
 * Constant defining the LED off state, relative to hardware.
 */
#define LED_OFF HIGH

void initLED();
void initLEDTimer();
void startLEDTimer();
void stopLEDTimer();
void setLED(int enabled);
void ICACHE_RAM_ATTR ledISR(void);
void setRedLED(int enabled);

#endif
