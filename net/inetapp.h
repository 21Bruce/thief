#ifndef __inetapplib__
#define __inetapplib__

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
#include <signal.h>
#include <setjmp.h>

#include "../lib/elib.h"

void parseurl(char *url, char *protocol, char *hostname,
	int *port, char *path);

void parsepath(char *path, char *filename, char **query,
	int *q_len);

void makeurl(char *url, char *protocol, char *hostname,
	int *port, char *filename, char **query, int q_len);

void get(char *url, char *header, size_t helen, char *rsc,
	size_t rlen);

/* FILE PRIVATE */

static void httptimeout(int signo); 

static void findcookies(char *header, char cookies[][512],
	int *cblen, int clen);

static int tcpconnect(int port, char *hostname);

static void makegetrequest(char *req, int rlen, char *path,
	char *hostname, char *version, char cookies[][512],
	int clen);

static int recvhttp(int fd, char *header, size_t helen,
	char *rsc, size_t rlen);

static void gethttps(char *url, char *header, size_t helen,
	char *rsc, size_t rlen);

static void gethttp(char *url, char *header, size_t helen,
	char *rsc, size_t rlen);

#endif /*__inetapplib__*/

