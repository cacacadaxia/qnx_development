#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <string.h>
#include <termios.h>

#ifndef NULL
#define NULL 0
#endif

typedef enum {false = 0, true = 1} bool;

int key_read();

#endif
