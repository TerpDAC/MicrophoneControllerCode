#include <stdarg.h>
#include <stdio.h>
#include "util.h"
#include <HardwareSerial.h>

void ser_p(const char *fmt, ... ) {
  char buf[512]; // resulting string limited to 512 chars
  va_list args;
  va_start (args, fmt);
#ifdef __AVR__
  vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
#else
  vsnprintf(buf, sizeof(buf), (const char *)fmt, args); // for the rest of the world
#endif
  va_end(args);
  Serial.print(buf);
}
