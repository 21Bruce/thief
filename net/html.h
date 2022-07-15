#ifndef __htmllib__
#define __htmllib__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#include "../lib/elib.h"

void linkparse(char *html, char *token, char links[][256], int *l_len, int pos); 

#endif /*__htmllib__ */
