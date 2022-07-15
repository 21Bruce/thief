#include "http.h"
#define URLLEN 256
#define HTMLLEN 32768 
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

static void makegetrequest(char *req, int req_len, char *path, char *hostname) {
	snprintf(req, req_len, "GET %s HTTP/1.1\r\n", path);	
	snprintf(req + strlen(req), req_len - strlen(req), "Host: %s\r\n", hostname); 
	snprintf(req + strlen(req), req_len - strlen(req), "\r\n");
}

static void recvhtml(int fd, char *html, size_t len) {
	char *end = html + len;
	char *curr = html;
	char *body = 0;
	char *strenc;
	enum {length, chunk, conn};
	int encoding = 0;
	int remaining = 0;
	int bytes_recv;
	while (1) {
		if ((bytes_recv = recv(fd, curr, end - curr, 0)) < 1) 
			if (body && encoding == conn) 
				break;
		curr += bytes_recv;			
		*curr = '\0';
		if (!body && (body = strstr(html, "\r\n\r\n")))
			body += 4;
		
		if ((strenc = strstr(html, "\nContent-Length: "))) {
			encoding = length; 
			strenc = strchr(strenc, ' ');
			strenc++;
			remaining = strtol(strenc, 0, 10);
		} else if ((strenc = strstr(html, "\nTransfer-Encoding: chunked"))) {
			encoding = chunk;
			remaining = 0;
		} else {
			encoding = conn;
		}
		
		if (encoding == length && curr-body >= remaining)
			break;
		if (encoding == chunk) {
			do {
				if (remaining == 0) {
					if ((strenc = strstr(body, "\r\n"))) {
						remaining = strtol(body, 0, 16);
						fprintf(stderr, "ChunLen: %d\n", remaining);
						if (!remaining) return;
						body = strenc + 2;
					} else {break;}
				}
				if (remaining && curr - body >= remaining) {
					body += remaining + 2;
					remaining = 0;
				}
			} while(!remaining);
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
	makegetrequest(req, 1024, path, hostname);
	int wwwconn = tcpconnect(port, hostname);		
	send(wwwconn, req, strlen(req), 0);
	char *html = (char *) malloc(HTMLLEN);
	memset(html, 0, HTMLLEN);
	recvhtml(wwwconn, html, HTMLLEN);
	close(wwwconn);
	return html;
}


int main (int argc, char *argv[]) {
	char protocol[32];
	char hostname[32];
	int port = 80;
	char path[256];
	parseurl(argv[1], protocol, hostname,
		&port, path);
	printf("path: %s\n", path);
}

