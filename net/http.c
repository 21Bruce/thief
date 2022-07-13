#include "http.h"
#define URLLEN 256
#define HTMLLEN 8192 
#define LEN(url) strlen(url)

void parseurl(char *u, char *p, char *h, int *pt,
	char *pa) {
	int size = strlen(u);
	char *curr = u;
	char *pstart = p; 
	while (*curr != ':' && curr - u < size) 
		*p++ = *curr++;
	*p = '\0';
	p = pstart;
	curr = curr + 3;
	char *hstart = h;
	while (*curr != ':' && curr - u < size) 
		*h++ = *curr++;
	curr++;
	*h = '\0';
	h = hstart;	
	char ptstr[6];
	int i;
	for (i = 0; *curr != '/'; i++)
		ptstr[i] = *curr++;
	ptstr[i+1] = '\0';
	*pt = atoi(ptstr);
	char *pastrt = pa;	
	while (curr - u < size)
		*pa++ = *curr++;
	*pa = '\0';
	pa = pastrt;	
}
		
void parsepath(char *p, char *f, char **q, int *q_len) {
	int size = strlen(p);	
	char *curr = p;
	char *fstart = f;
	while (curr - p < size && *curr != '?')
		*f++ = *curr++;
	*f = '\0';
	f = fstart;
	int arg;
	if (*curr == '?') {
		*q_len = 0;
		while (curr - p < size) {
			curr++;
			*q_len += 1;
			arg = (*q_len) - 1;		
			char *qcurr = q[arg];
			while (curr - p < size && *curr != '&')
				*qcurr++ = *curr++;
			*qcurr = '\0';
		}
	} else {
		*q_len = 0;
	}
}

char *makeurl(char *p, char *h, int *pt, char *f, char **q, int q_len) {
	char *url = (char *) malloc(URLLEN);
	snprintf(url, URLLEN, "%s://", p);
	snprintf(url + LEN(url), URLLEN - LEN(url), "%s", h);
	if (pt) snprintf(url + LEN(url), URLLEN - LEN(url), ":%d", *pt);
	snprintf(url + LEN(url), URLLEN - LEN(url), "%s", f);
	if (q_len) snprintf(url + LEN(url), URLLEN - LEN(url), "?");
	char **curr; 
	for (int i = 0; i < q_len; i++) {	
		curr = q + i;
		snprintf(url + LEN(url), URLLEN - LEN(url), "%s", *curr);
		if (i != q_len-1) snprintf(url + LEN(url), URLLEN - LEN(url), "&");
	}
	return url;
}

static int tcpconnect(int port, char *hostname) {
	char strport[32];	
	snprintf(strport, sizeof(strport), "%d", port);
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo *addr;
	if (getaddrinfo(hostname, strport, &hints, &addr))
		fatal();
	int tcpsocket = socket(addr->ai_family,
				addr->ai_socktype,
				addr->ai_protocol);
	if (tcpsocket < 0) fatal();
	if (connect(tcpsocket, addr->ai_addr, addr->ai_addrlen))
		fatal();
	freeaddrinfo(addr);
	return tcpsocket;
}	

static void makegetrequest(char *req, int req_len, char *path, char *hostname) {
	snprintf(req, req_len, "GET %s HTTP/1.1\r\n", path);	
	snprintf(req + strlen(req), req_len - strlen(req), "Host: %s\r\n", hostname); 
	snprintf(req + strlen(req), req_len - strlen(req), "\r\n");
}

char *gethttpurl(char *u) {
	char protocol[32];
	char hostname[32];
	int port;
	char path[256];
	parseurl(u, protocol, hostname,
		&port, path);
	fprintf(stderr, "protocol: %s\n", protocol);
	fprintf(stderr, "hostname: %s\n", hostname);
	fprintf(stderr, "port: %d\n", port);
	fprintf(stderr, "path: %s\n", path);
	char req[1024];
	makegetrequest(req, 1024, path, hostname);
	printf("Req:\n%s\n", req);
	int wwwconn = tcpconnect(port, hostname);		
	send(wwwconn, req, strlen(req), 0);
	char *res = (char *) malloc(HTMLLEN);
	ssize_t reslen;
	while ((reslen = recv(wwwconn, res, HTMLLEN, 0)) > 0)
		printf("%.*s\n", (int) reslen, res);  	
	return res;	
}

int main () {
	
	char *url = "https://www.google.com:80/";	
	gethttpurl(url);
}
