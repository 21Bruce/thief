#include "inrsc_cfg.h"

static void _parseurl(char *u, char **p, size_t *p_len,
	char **h, size_t *h_len, int *pt, char **pa, size_t *pa_len) {
	int size = strlen(u);
	char *curr = u;
	char *fcurr = *p; 
	while (*curr != ':' && curr - u < size) { 
		if (fcurr - *p == *p_len - 1) {
			*p = realloc(*p, 2 * (*p_len));
			fcurr = *p + *p_len - 1;	
			*p_len = 2 * (*p_len);
		}
		*fcurr++ = *curr++;
	}
	*fcurr = '\0';
	curr = curr + 3;
	fcurr = *h;
	while (*curr != ':' && *curr != '/' && curr - u < size) { 
		if (fcurr - *h == *h_len - 1) {
			*h = realloc(*h, 2 * (*h_len));
			fcurr = *h + *h_len - 1;	
			*h_len = 2 * (*h_len);
		}
		*fcurr++ = *curr++;
	}
	*fcurr = '\0';
	if (*curr == ':') {
		curr++;
		char ptstr[6];
		int i;
		for (i = 0; *curr != '/'; i++)
			ptstr[i] = *curr++;
		ptstr[i+1] = '\0';
		*pt = atoi(ptstr);
	} else {
		if (strcmp(*p, "http") == 0) *pt = 80;
		if (strcmp(*p, "https") == 0) *pt = 443;
	}
	fcurr = *pa;	
	while (curr - u < size & *curr != '#') {
		if (fcurr - *pa == *pa_len - 1) {
			*pa = realloc(*pa, 2 * (*pa_len));
			fcurr = *pa + *pa_len - 1;	
			*pa_len = 2 * (*pa_len);
		}
		*fcurr++ = *curr++;
	}
	*fcurr = '\0';
}

static void _mallocurl(INRSC_CFG *cfg, char *url) {
	if (!cfg) return;
	if(!cfg->protocol) { 
		cfg->protocol_len = _DEFAULT_PROTOCOL_LEN;
		cfg->protocol = (char *) malloc(cfg->protocol_len);
	}
	if(!cfg->hostname) { 
		cfg->hostname_len = _DEFAULT_HOSTNAME_LEN;
		cfg->hostname = (char *) malloc(cfg->hostname_len);
	}
	if(!cfg->path) {
		cfg->path_len = _DEFAULT_PATH_LEN;
		cfg->path = (char *) malloc(cfg->path_len);
	}	
	_parseurl(url, &(cfg->protocol), &(cfg->protocol_len),
		&(cfg->hostname), &(cfg->hostname_len), 
		&(cfg->port), &(cfg->path), &(cfg->path_len)); 
}

static void _mallocdir(INRSC_CFG *cfg, char *dir) {
	if (!cfg) return;
	if (!cfg->dir) {
		cfg->dir_len = _DEFAULT_DIR_LEN;
		cfg->dir = (char *) malloc(cfg->dir_len);
	}
	if (dir == NULL) return; 
	size_t dir_len = strlen(dir);
	if (dir_len > cfg->dir_len) {
		cfg->dir = (char *) realloc(cfg->dir, dir_len);
		cfg->dir_len = dir_len;
	}
	strcpy(cfg->dir, dir);

}

int INRSC_CFG_new(INRSC_CFG **c) {
	INRSC_CFG *cfg = (INRSC_CFG *) malloc(sizeof(INRSC_CFG));
	*c = cfg;
	cfg->protocol_len = _DEFAULT_PROTOCOL_LEN;
	cfg->protocol = (char *) malloc(cfg->protocol_len);
	cfg->hostname_len = _DEFAULT_HOSTNAME_LEN;
	cfg->hostname = (char *) malloc(cfg->hostname_len);
	cfg->path_len = _DEFAULT_PATH_LEN;
	cfg->path = (char *) malloc(cfg->path_len);
	return 0;
}

int INRSC_CFG_free(INRSC_CFG *cfg) {
	if (!cfg) return 1;
	if (cfg->protocol != NULL) free(cfg->protocol);
	if (cfg->hostname != NULL) free(cfg->hostname);
	if (cfg->path != NULL) free(cfg->path);
	if (cfg->dir != NULL) free(cfg->dir);
	cfg->port = 0;
	free(cfg);
	return 0;
}

int INRSC_CFG_urlcfg(INRSC_CFG *cfg, char *url) {
	if (!cfg) return 1;
	if (!url) return 1;
	_mallocurl(cfg, url);
	return 0;
}

int INRSC_CFG_getprotocol(INRSC_CFG *cfg, char *protocol, size_t *len) {
	if (!cfg) return 1;
	if (*len == 0 || protocol == NULL) {
		*len = strlen(cfg->protocol);
		return 0;
	}
	size_t min_len = strlen(cfg->protocol) + 1; 
	if (min_len > (*len)) min_len = *len;
	strncpy(protocol, cfg->protocol, min_len);
	protocol[min_len-1] = '\0';
	return 0;
}
			
int INRSC_CFG_setprotocol(INRSC_CFG *cfg, char *protocol) {
	if (!cfg) return 1;
	if (protocol == NULL) return 1;
	size_t protocol_len = strlen(protocol);
	if (protocol_len > cfg->protocol_len) {	
		cfg->protocol = (char *) realloc(cfg->protocol, protocol_len); 
		cfg->protocol_len = protocol_len;
	}
	strcpy(cfg->protocol, protocol);
	return 0;
}

int INRSC_CFG_gethostname(INRSC_CFG *cfg, char *hostname, size_t *len) {
	if (!cfg) return 1;
	if (*len == 0 || hostname == NULL) {
		*len = strlen(cfg->hostname);
		return 0;
	}
	
	size_t min_len = strlen(cfg->hostname) + 1; 
	if (min_len > (*len)) min_len = *len;
	strncpy(hostname, cfg->hostname, min_len);
	hostname[min_len-1] = '\0';
	return 0;
}
			
int INRSC_CFG_sethostname(INRSC_CFG *cfg, char *hostname) {
	if (hostname == NULL) return 1;
	size_t hostname_len = strlen(hostname);
	if (hostname_len > cfg->hostname_len) {	
		cfg->hostname = (char *) realloc(cfg->hostname, hostname_len); 
		cfg->hostname_len = hostname_len;
	}
	strcpy(cfg->hostname, hostname);
	return 0;
}

int INRSC_CFG_getport(INRSC_CFG *cfg, int *port) {
	if (!cfg) return 1;
	*port = cfg->port;
	return 0;
}

int INRSC_CFG_setport(INRSC_CFG *cfg, int port) {
	if (!cfg) return 1;
	cfg->port = port;
	return 0;
}

int INRSC_CFG_getpath(INRSC_CFG *cfg, char *path, size_t *len) {
	if (!cfg) return 1;
	if (*len == 0 || path == NULL) {
		*len = strlen(cfg->path);
		return 0;
	}
	size_t min_len = strlen(cfg->path) + 1; 
	if (min_len > (*len)) min_len = *len;
	strncpy(path, cfg->path, min_len);
	path[min_len-1] = '\0';
	return 0;
}
			
int INRSC_CFG_setpath(INRSC_CFG *cfg, char *path) {
	if (!cfg) return 1;
	if (path == NULL) return 1;
	size_t path_len = strlen(path);
	if (path_len > cfg->path_len) {	
		cfg->path = (char *) realloc(cfg->path, path_len); 
		cfg->path_len = path_len;
	}
	strcpy(cfg->path, path);
	return 0;
}

int INRSC_CFG_getdir(INRSC_CFG *cfg, char *dir, size_t *len) {
	if (!cfg) return 1;
	if (!cfg->dir_len || !cfg->dir) return 1;
	if (*len == 0 || dir == NULL) {
		*len = strlen(cfg->dir);
		return 0;
	}
	size_t min_len = strlen(cfg->dir) + 1; 
	if (min_len > (*len)) min_len = *len;
	strncpy(dir, cfg->dir, min_len);
	dir[min_len-1] = '\0';
	return 0;
}

int INRSC_CFG_setdir(INRSC_CFG *cfg, char *dir) {
	if (!cfg) return 1;
	if (dir == NULL) return 1;
	if (!cfg->dir) {
		_mallocdir(cfg, dir);
		return 0;
	}
	size_t dir_len = strlen(dir) + 1; 
	if (dir_len > cfg->dir_len) {
		cfg->dir = (char *) realloc(cfg->dir, dir_len);
		cfg->dir_len = dir_len;
	}
	strcpy(cfg->dir, dir);
	return 0;
}

