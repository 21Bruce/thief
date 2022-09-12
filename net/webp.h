#ifndef __webp__
#define __webp__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include "inrsc.h"
#include "inrsc_cfg.h"

#include "../lib/elib.h"

INRSC *get(INRSC_CFG *cfg);

#endif /*__webp__*/
