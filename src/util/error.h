#ifndef SRC_ERROR_H_
#define SRC_ERROR_H_

#include <cstdarg>

// Prints an error and cleans up and exits the program.
void error(const char *fmt, ...);

void warning(const char *fmt, ...);

void checkForGLError();

#endif  // SRC_ERROR_H_