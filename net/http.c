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

static int findcookies(char *header, char *cookies, int clen) {
	char *strt = strstr(header, "\nSet-Cookie: ");	
	if (!strt) strt = strstr(header, "\nset-cookie: ");	
	if (!strt) return 0;
	strt += 13; 
	char *end = strstr(strt, "\r\n");
	char *curr = strt;
	while (curr != end && curr < strt + clen)
		*cookies++ = *curr++;
	return 1;
}
	
static int parsecookie(char *header, char cookies[][256], int *c_len) {
	return 0;
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
		 char * version, char *cookies)  {
	snprintf(req, req_len, "GET %s HTTP/%s\r\n", path, version);	
	snprintf(req + strlen(req), req_len - strlen(req), "Host: %s\r\n", hostname); 
	if (cookies)
		snprintf(req + strlen(req), req_len - strlen(req),
				 "Cookie: %s\r\n", cookies);
	snprintf(req + strlen(req), req_len - strlen(req), "\r\n");
}

static int recvhttp(int fd, char *header, size_t helen, char *html, size_t htlen) {
	int rlen = 4096;
	char rstr[rlen]; 
	memset(rstr, 0, rlen);
	char *h = header;
	char *p = html;
	char *chstrt;
	char *chend;
	char *rend;
	char *tmp;
	int bfound = 0;
	int rchln;
	enum {length=1, chunk=2};
	int encoding = 0;
	int remaining = 0;
	int status = 0;
	while ((rchln = recv(fd, rstr, rlen, 0)) > 1) {
		rend = rstr + rchln;
		chstrt = rstr;

		// Header loading 
		if (!bfound && !(chstrt = strstr(rstr, "\r\n\r\n"))) { 
			memcpy(h, rstr, rchln);
			h += rchln;
		} else if (!bfound) {
			bfound = 1;
			*chstrt = '\0';
			chstrt += 4;
			strcpy(h, rstr);
		}

		if (!bfound) continue;

		// Status scanning
		if (!status && (tmp = strchr(header, ' '))) 
			status = strtol(tmp, 0, 10); 
		
		// Encoding scanning
		if (!encoding && (tmp = strstr(header, "\nContent-Length: "))
				|| (tmp = strstr(header, "\ncontent-length: "))) {
			tmp = strchr(tmp, ' ');
			remaining = strtol(tmp, 0, 10);
			encoding = length;
		}

		if (!encoding && (tmp = strstr(header, "\nTransfer-Encoding: chunked"))
				|| (tmp = strstr(header, "\ntransfer-encoding: chunked"))) 
			encoding = chunk;	
		
		// Chunk processing
		if (encoding == length) {
			int llen = rchln - (chstrt-rstr);
			memcpy(p, chstrt, llen);
			p += llen;
			*p = '\0';
			remaining -= llen;			
			if (remaining <= 0) return status;
		}

		if (encoding == chunk) {
			int chln;
			while (1) {
				if (!remaining) {
					chln = rend - chstrt;
					remaining = strtol(chstrt, 0, 16);
					if (!remaining) return status;
					chstrt = strstr(chstrt, "\r\n") + 2;
				} 
				if (remaining) {
					chln = rend - chstrt;	
					if (chln < remaining) {
						memcpy(p, chstrt, chln);
						p += chln;
						*p = '\0';
						remaining -= chln;
						break;
					}
						
					chend = strnstr(chstrt, "\r\n", chln);
					while (chend - chstrt != remaining) {
						chend += 2;
						chend = strnstr(chend, "\r\n", rend-chend);
					}
							
					chln = chend - chstrt;
					memcpy(p, chstrt, chln);
					p += chln;
					*p = '\0';
					remaining -= chln;
					chstrt = chend + 2;
					if (chstrt >= rend) break;
				}
			}
		}
		memset(rstr, 0, rlen);
	}
	return status;
}
				 
void gethttp(char *u, char *header, size_t helen, char *html, size_t htlen) {
	char *version = "1.1";
	char *url = u;
	char protocol[32];
	char hostname[32];
	char *cookies = NULL;
	int c_len = 5;
	int port = 80;
	char path[512];
	int status = 0;
	int wwwconn;
	char req[1024];
	while (status != 200) {
		parseurl(url, protocol, hostname,
			&port, path);
		wwwconn = tcpconnect(port, hostname);		
		makegetrequest(req, 1024, path, hostname, 
				version, cookies);
		send(wwwconn, req, strlen(req), 0);
		status = recvhttp(wwwconn, header, helen, html, htlen);
		if (status / 100 == 5) {
			close(wwwconn);
			if (!strcmp(version, "1.1")) version = "2";
			else if (!strcmp(version, "2")) version = "1.1";
			continue;
		}
		if (status / 100 == 3) {
			close(wwwconn);
			if (!cookies) {
				cookies = (char *) malloc (256);
				if (!findcookies(header, cookies, 256)) {
					free(cookies);  
					cookies = NULL; 
				}
			}
			char *urlstrt = strstr(header, "\nLocation: ");
			if (!urlstrt) urlstrt = strstr(header, "\nlocation: ");
			urlstrt += 11;
			char *urlend = strstr(urlstrt, "\r\n");
			*urlend = '\0';
			url = urlstrt;
			continue;
		}
		break;
	}
	close(wwwconn);
}

int main(int argc, char *argv[]) {

	char *header = (char *) malloc(1024);
	char *html = (char *) malloc(HTMLLEN);
	gethttp(argv[1], header, 1024, html, HTMLLEN);
	printf("%s\n%s\n", header, html);
	free(header);
	free(html);
}
