/*
 * Terrapin Development and Consulting (TerpDAC)
 * Microphone-Traffic Mapping Project
 * Microphone Initial Testing Program
 * Written by William Heimsoth
 * Created: 11/29/2015
 * Last modified: 11/30/2015
 */

// Define to activate lower precision, faster sample rate
// ADC conversion (visit http://forum.arduino.cc/index.php?topic=6549.0)
#define FASTADC 1
// Send number of samples, begin time of samples, and end time of samples
#define SENDTIME 1
// Print number of samples and difference between end time and begin time of samples
//#define PRINTTIME 1
// Wait for ack from testing program to signal data collection
//#define WAITFORACK 1

// Definitions required to set and clear bits at AVR level
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Constants:
const int micPin = A0;
const int numberSamples = 300;
String ackString = "Begin";

void setup()
{
  Serial.begin(9600);
#if FASTADC
  // Set ADC prescale to 32 (normally 128) for higher sample rate
  // but lower precision (~9.6kHz => ~33kHz)
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  sbi(ADCSRA, ADPS0);
#endif
#if WAITFORACK
  while (!(Serial.find(ackString))) {}
#endif
}

void loop() {
  // Add a variable to see if the state has changed/need to sendAnalysis
  int samples[numberSamples];
  int i = 0;
  unsigned long beginTime = 0, endTime = 0;
#if SENDTIME
  beginTime = micros();
#endif
  for (i = 0; i < numberSamples; i++) {
    samples[i] = analogRead(micPin);
  }
#if SENDTIME
  endTime = micros();
  Serial.println(String(numberSamples));
  Serial.println(String(beginTime));
  Serial.println(String(endTime));
#endif
  for (i = 0; i < numberSamples; i++) {
    //output = String(samples[i]);
    Serial.println(String(samples[i]));
#if PRINTTIME
    Serial.print("Samples taken = ");
    Serial.println(numberSamples);
    Serial.print("Time in microseconds = ");
    Serial.println(endTime - beginTime);
#endif
  }
