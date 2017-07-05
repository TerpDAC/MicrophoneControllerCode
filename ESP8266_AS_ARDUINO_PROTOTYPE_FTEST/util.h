#ifndef _UTIL_H
#define _UTIL_H

extern "C" {
  #include <cont.h>
  extern cont_t g_cont;
}

#include <Arduino.h>

void ser_p(const char *fmt, ... );

#ifdef DEBUG_PRINT
#define DPRINT(fmt, ...) ser_p("[%s:%d] [%s] " fmt, __FILE__,__LINE__,__FUNCTION__,##__VA_ARGS__)
#else
#define DPRINT(fmt, ...)
#endif

/* Define memory saving serial print functions. */
#define SerialPrintStr(...) Serial.print(F(__VA_ARGS__))
#define SerialPrintStrLn(...) Serial.println(F(__VA_ARGS__))

/* Define configuration related functions. */
#define PRINT_FOREVER(...) while (1) { SerialPrintStrLn(__VA_ARGS__); delay(2000); }

/* Stack statistics function - print the current stack status */
register uint32_t *sp asm("a1");

#define DUMP_STACK_STATS() SerialPrintStr("unmodified stack   = "); Serial.println(cont_get_free_stack(&g_cont)); \
  SerialPrintStr("current free stack = "); Serial.println(4 * (sp - g_cont.stack))

#endif
