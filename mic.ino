/*
 * Terrapin Development and Consulting (TerpDAC)
 * Microphone-Traffic Mapping
 * Written by William Heimsoth
 * Created: 11/29/2015
 * Last modified: 11/30/2015
 * Basic Algorithm:
 * (1) Initialize
 * (2) Sample the mic as fast as possible
 * (3) Analyze collected data
 * (4) Send analyzed data
 * (5) Repeat from (2)
 */

// Define to activate lower precision, faster sample rate
// ADC conversion (visit http://forum.arduino.cc/index.php?topic=6549.0)
#define FASTADC 1
// Definitions required to set and clear bits at AVR level
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Constants:
const int micPin = A0;
const int numberSamples = 1000;
const int lowThreshold = 200;
const int highThreshold = 400;
const int levelOne = 1;
const int levelTwo = 2;
const int levelThree = 3;
// Variables:


void setup()
{
  Serial.begin(9600);
#if FASTADC
  // Set ADC prescale to 16 (normally 128) for higher sample rate
  // but lower precision (~9.6kHz => ~77kHz)
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);
#endif
}

void loop() {
  // Add a variable to see if the state has changed/need to sendAnalysis
  int samples[numberSamples], level = 0;
  collectData(samples);
  level = analyzeData(samples);
  sendAnalysis(level);
}

void collectData(int samples[]) {
  int i = 0;
  for (i = 0; i < numberSamples; i++) {
    samples[i] = analogRead(micPin);  
  }
}

int analyzeData(int samples[]) {
  int i = 0, average = 0, level = 0;
  // Map the values from -512 to 511 and take the absolute value
  /*for (i = 0; i < numberSamples; i++) {
    samples[i] = map(samples[i], 0, 1023, -512, 511);
  }*/
  // Mapping can also be accomplished with subtraction
  for (i = 0; i < numberSamples; i++) {
    samples[i] -= 512;
    samples[i] = abs(samples[i]);
  }
  // Assume that the samples are all noise, so take the average
  for (i = 0; i < numberSamples; i++) {
    average += samples[i];
  }
  average /= numberSamples;
  if (average < lowThreshold) {
    level = levelOne;
  }
  else if (average > lowThreshold && average < highThreshold) {
    level = levelTwo;
  }
  else {
    level = levelThree;
  }
  return level;
}

void sendAnalysis(int level) {
  // Wifi stuff
}
