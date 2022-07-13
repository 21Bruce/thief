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
#include <errno.h>

#include "../lib/elib.h"

typedef struct {
	char *protocol;
	char *hostname;
	int *port;
	char *filename;
	char **query;
} urlent; 

void parseurl(char *url, char *protocol, char *hostname,
	int *port, char *path);

void parsepath(char *path, char *filename, char **query,
	int *q_len);

char* makeurl(char *protocol, char *hostname,
	int *port, char *filename, char **query, int q_len);

char *gethttpurl(char *url);


#endif /*__httplib__*/

