#include "error.h"

#include <cstdio>

// Forward declare our exit point.
void cleanupAndExit(int exit_code);

void error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  cleanupAndExit(1);
}