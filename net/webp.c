#include "webp.h"

static INRSC *_gethttps(INRSC_CFG *cfg);
static INRSC *_gethttp(INRSC_CFG *cfg);
static int _tcpconnect(INRSC_CFG *cfg);
static void _makegetrequest(char *req, int req_len, INRSC_CFG *cfg, 
			char *version, char cookies[][512],
			int clen);

INRSC *get(INRSC_CFG *cfg) {
	size_t p_len;
	INRSC_CFG_getprotocol(cfg, NULL, &p_len);
	char protocol[p_len];
	INRSC_CFG_getprotocol(cfg, protocol, &p_len);

	if (strcmp(protocol, "https") == 0) return _gethttps(cfg) 
	if (strcmp(protocol, "http") == 0) return _gethttp(cfg);
	return NULL;
}	

static int _tcpconnect(INRSC_CFG *cfg) {
	int port;
	INRSC_CFG_getport(cfg, &port);

	char strport[7];	
	snprintf(strport, sizeof(strport), "%d", port);

	size_t h_len;
	INRSC_CFG_gethostname(cfg, NULL, &h_len);
	char hostname[h_len];
	INRSC_CFG_gethostname(cfg, hostname, &h_len);

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

static void _makegetrequest(char *req, int req_len, INRSC_CFG *cfg, 
		 char *version, char cookies[][512], int clen)  {
	snprintf(req, req_len, "GET %s HTTP/%s\r\n", path, version);	
	snprintf(req + strlen(req), req_len - strlen(req), "Host: %s\r\n", hostname); 
	if (clen)
		for (int i = 0; i < clen; i++)	
			snprintf(req + strlen(req), req_len - strlen(req),
				 "Cookie: %s\r\n", cookies[i]);
	snprintf(req + strlen(req), req_len - strlen(req), "\r\n");
}

INRSC *_gethttp(INRSC_CFG *cfg) {
}	

