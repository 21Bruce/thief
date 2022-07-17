#ifndef __httplib__
#define __httplib__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <curl/curl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <setjmp.h>

#include "../lib/elib.h"

void parseurl(char *url, char *protocol, char *hostname,
	int *port, char *path);

void parsepath(char *path, char *filename, char **query,
	int *q_len);

char* makeurl(char *protocol, char *hostname,
	int *port, char *filename, char **query, int q_len);

void gethttpurl(char *url, char **header, char **html);


#endif /*__httplib__*/

