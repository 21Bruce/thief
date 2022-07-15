#include "http.h"
#define URLLEN 256
#define HTMLLEN 100000  
#define TIMEOUT 5
#define LEN(url) strlen(url)

static sigjmp_buf e;

static void httptimeout(int signo) {
	siglongjmp(e, 1);
}

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
	while (*curr != ':' && *curr != '/' && curr - u < size) 
		*h++ = *curr++;
	*h = '\0';
	h = hstart;	
	if (*curr == ':') {
		curr++;
		char ptstr[6];
		int i;
		for (i = 0; *curr != '/'; i++)
			ptstr[i] = *curr++;
		ptstr[i+1] = '\0';
		*pt = atoi(ptstr);
	} 
	char *pastrt = pa;	
	while (curr - u < size & *curr != '#')
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
	*q_len = 0;
	if (*curr == '?') 
		while (curr - p < size) {
			curr++;
			*q_len += 1;
			arg = (*q_len) - 1;		
			char *qcurr = q[arg];
			while (curr - p < size && *curr != '&')
				*qcurr++ = *curr++;
			*qcurr = '\0';
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

static void makegetrequest(char *req, int req_len, char *path, char *hostname,
		 char * version) {
	snprintf(req, req_len, "GET %s HTTP/%s\r\n", path, version);	
	snprintf(req + strlen(req), req_len - strlen(req), "Host: %s\r\n", hostname); 
	snprintf(req + strlen(req), req_len - strlen(req), "\r\n");
}

static void recvhtml(int fd, char *html, size_t len) {
	if (sigsetjmp(e, 1) == 1) return;
	signal(SIGALRM, httptimeout); 

	int chlen;
	char *bend = html + len;
	char *cend = html;
	char *chstrt;
	char *chend;
	char *tmp;
	char *body = 0;
	enum {chunk=1, length=2};
	int encoding = 0;
	int remaining = 0;

	alarm(TIMEOUT);
	while ((chlen = recv(fd, cend, bend-cend, 0)) > 1) {
		cend += chlen;	
		*cend = '\0';
		if (!body && (body = strstr(html, "\r\n\r\n"))) {
			body += 2;
			chend = body;
		} 
		if (!encoding && (tmp = strstr(html, "\nContent-Length: "))) {
			encoding = length;
			tmp = strchr(tmp, ' ');
			remaining = strtol(tmp, 0, 10);
			body += 2;
		}
		if (!encoding && (tmp = strstr(html, "\nTransfer-Encoding: chunked")))
			encoding = chunk;
		
		if (!encoding)
			body += 2;
			
		if (encoding == length && cend-body >= remaining) 
			return;

		if (encoding == chunk) {
			if (!remaining && (chstrt = strstr(chend, "\r\n"))) {
				chstrt += 2;
				remaining = strtol(chstrt, 0, 16);
				if (!remaining) return;
				chstrt = strstr(chstrt, "\r\n") + 2;
			}
			if (remaining && (chend = strstr(chstrt, "\r\n"))) {
				remaining -= chend-chstrt;
			}
		}
		
	}
}

char *gethttpurl(char *u) {
	char protocol[32];
	char hostname[32];
	int port = 80;
	char path[256];
	parseurl(u, protocol, hostname,
		&port, path);
	char req[1024];
	makegetrequest(req, 1024, path, hostname, "1.1");
	int wwwconn = tcpconnect(port, hostname);		
	send(wwwconn, req, strlen(req), 0);
	char *html = (char *) malloc(HTMLLEN);
	memset(html, 0, HTMLLEN);
	recvhtml(wwwconn, html, HTMLLEN);
	close(wwwconn);
	return html;
}


