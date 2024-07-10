#ifndef __SYSCLIB_H___
#define __SYSCLIB_H___

#include <pspsdk.h>
#include <pspsysclib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

int vsnprintf(char *buf, size_t n, const char *fmt, va_list ap);
int snprintf(char *buf, size_t n, const char *fmt, ...);

#endif

