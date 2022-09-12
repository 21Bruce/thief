#ifndef __inrsc_cfg__
#define __inrsc_cfg__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* INRSC_CFG: Internet Resource Config */

/* Constants */
#define _DEFAULT_PROTOCOL_LEN 8
#define _DEFAULT_HOSTNAME_LEN 32 
#define _DEFAULT_PATH_LEN 512 
#define _DEFAULT_DIR_LEN 64


typedef struct {
	size_t protocol_len;
	char *protocol;
	size_t hostname_len;
	char *hostname;
	int port;
	size_t path_len;
	char *path;
	size_t dir_len;
	char *dir;
} INRSC_CFG; 		
	
// Setup/Destruction
int INRSC_CFG_new(INRSC_CFG **cfg);
int INRSC_CFG_free(INRSC_CFG *cfg);

// Setters/Getters
int INRSC_CFG_urlcfg(INRSC_CFG *cfg, char *url);
int INRSC_CFG_getprotocol(INRSC_CFG *cfg, char *protocol, size_t *len);
int INRSC_CFG_setprotocol(INRSC_CFG *cfg, char *protocol);
int INRSC_CFG_gethostname(INRSC_CFG *cfg, char *hostname, size_t *len);
int INRSC_CFG_sethostname(INRSC_CFG *cfg, char *hostname);
int INRSC_CFG_getport(INRSC_CFG *cfg, int *port);
int INRSC_CFG_setport(INRSC_CFG *cfg, int port);
int INRSC_CFG_getpath(INRSC_CFG *cfg, char *path, size_t *len);
int INRSC_CFG_setpath(INRSC_CFG *cfg, char *path);
int INRSC_CFG_getdir(INRSC_CFG *cfg, char *dir, size_t *len);
int INRSC_CFG_setdir(INRSC_CFG *cfg, char *dir);

#endif /*__inrsc_cfg__*/
