#ifndef __inrsc__
#define __inrsc__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define _DEFAULT_HEADER_LEN 512
#define _DEFAULT_HTML_LEN 100000
#define _DEFAULT_DIR_LEN 64
#define _DEFAULT_FILENAME_LEN 64

/* INRSC: Internet Resource */

typedef enum { DJVU, PDF, CHM, GZ, FT_UNKNOWN } FILE_TYPE;

typedef enum { UNKNOWN, DFILE, HTML, ERROR } RSC_TYPE;

typedef enum { } INRSC_ERROR_CODE;

typedef struct {
	FILE_TYPE ft;
	size_t dir_len;
	char *dir;
	size_t filename_len;
	char *filename;
} FILE_RSC; 

typedef struct {
	size_t html_len;
	char *html;
} HTML_RSC;

typedef struct {
	INRSC_ERROR_CODE errcode;
} ERROR_RSC;

typedef union {
	FILE_RSC *frsc;
	HTML_RSC *hrsc;
	ERROR_RSC *ersc;
} RSC;

typedef struct {
	size_t header_len;
	char *header;
	RSC_TYPE rsc_type;
	RSC *rsc;
} INRSC;

//Init/free
int INRSC_new(INRSC **rsc);
int INRSC_free(INRSC *rsc);

// Processing
int INRSC_isfile(INRSC *irsc);
int INRSC_ishtml(INRSC *irsc);
int INRSC_isunknown(INRSC *irsc);
int INRSC_getfilename(INRSC *irsc, char *filename, size_t *len);
int INRSC_getdir(INRSC *irsc, char *dir, size_t *len);
int INRSC_gethtml(INRSC *irsc, char *html, size_t *len);
int INRSC_getheader(INRSC *irsc, char *header, size_t *len);
int INRSC_setfilename(INRSC *irsc, char *filename);
int INRSC_setdir(INRSC *irsc, char *dir);
int INRSC_sethtml(INRSC *irsc, char *html);
int INRSC_setheader(INRSC *irsc, char *header);
int INRSC_settypefile(INRSC *irsc);
int INRSC_settypehtml(INRSC *irsc);
int INRSC_setfiletype(INRSC *irsc, FILE_TYPE file_type);

#endif /*__inrsc__*/
