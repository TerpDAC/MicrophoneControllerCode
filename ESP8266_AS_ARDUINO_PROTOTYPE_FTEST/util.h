#ifndef _UTIL_H
#define _UTIL_H

void ser_p(const char *fmt, ... );

#ifdef DEBUG_PRINT
#define DPRINT(fmt, ...) ser_p("[%s:%d] [%s] " fmt, __FILE__,__LINE__,__FUNCTION__,##__VA_ARGS__)
#else
#define DPRINT(fmt, ...)
#endif

#endif
