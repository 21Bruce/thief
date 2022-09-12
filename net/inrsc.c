#include "inrsc.h"

int INRSC_new(INRSC **irsc) {
	INRSC *r = (INRSC *) malloc(sizeof(INRSC));
	*irsc = r; 
	r->header_len = _DEFAULT_HEADER_LEN;
	r->header = (char *) malloc(r->header_len);
	r->rsc_type = UNKNOWN;
	r->rsc = NULL;
	return 0;
}

static void _freehtml(INRSC *irsc) {
	if (!irsc->rsc->hrsc) return;
	if (irsc->rsc->hrsc->html) free(irsc->rsc->hrsc->html);
	free(irsc->rsc->hrsc);
}

static void _freefile(INRSC *irsc) {
	if (!irsc->rsc->frsc) return;
	if (irsc->rsc->frsc->dir) free(irsc->rsc->frsc->dir);
	if (irsc->rsc->frsc->filename) free(irsc->rsc->frsc->filename);
	free(irsc->rsc->frsc);
}

static void _allocfile(INRSC *irsc) {
	irsc->rsc_type = DFILE;
	irsc->rsc = (RSC *) malloc(sizeof(RSC));
	irsc->rsc->frsc = (FILE_RSC *) malloc(sizeof(FILE_RSC)); 
	irsc->rsc->frsc->ft = FT_UNKNOWN;
	irsc->rsc->frsc->dir_len = _DEFAULT_DIR_LEN;
	irsc->rsc->frsc->dir = (char *) malloc(irsc->rsc->frsc->dir_len);
	irsc->rsc->frsc->filename_len = _DEFAULT_FILENAME_LEN;
	irsc->rsc->frsc->filename = (char *) malloc(irsc->rsc->frsc->filename_len);
}

static void _allochtml(INRSC *irsc) {
	irsc->rsc_type = HTML;
	irsc->rsc = (RSC *) malloc(sizeof(RSC));
	irsc->rsc->hrsc = (HTML_RSC *) malloc(sizeof(HTML_RSC)); 
	irsc->rsc->hrsc->html_len = _DEFAULT_HTML_LEN; 
	irsc->rsc->hrsc->html = (char *) malloc(irsc->rsc->hrsc->html_len); 
}

int INRSC_free(INRSC *irsc) {
	if (irsc == NULL) return 0;
	if (irsc->header != NULL) free(irsc->header);
	switch(irsc->rsc_type) {
		case DFILE:
			_freefile(irsc);
			break;
		case HTML:
			_freehtml(irsc);
			break;
		case UNKNOWN:
			break;
		default:
			break;
	}
	if (irsc->rsc != NULL) free(irsc->rsc);
	free(irsc);
	return 0;
}

int INRSC_isfile(INRSC *irsc) {
	if (irsc == NULL) return 0;
	return (irsc->rsc_type == DFILE);	
}

int INRSC_ishtml(INRSC *irsc) {
	if (irsc == NULL) return 0;
	return (irsc->rsc_type == HTML);	
}

int INRSC_isunknown(INRSC *irsc) {
	if (irsc == NULL) return 0;
	return (irsc->rsc_type == UNKNOWN);
}

int INRSC_getfilename(INRSC *irsc, char *filename, size_t *len) {
	if (!INRSC_isfile(irsc)) return 1;

	char *i_filename = irsc->rsc->frsc->filename;
	size_t filename_len = strlen(i_filename) + 1;
	if (filename == NULL || *len == 0) {
		*len = filename_len;
		return 0;
	}
	size_t min_len = filename_len > *len ? *len : filename_len;
	strncpy(filename, i_filename, min_len-1);
	filename[min_len-1] = '\0';
	return 0;
}

int INRSC_getdir(INRSC *irsc, char *dir, size_t *len) {
	if (!INRSC_isfile(irsc)) return 1;

	char *i_dir = irsc->rsc->frsc->dir;
	size_t dir_len = strlen(i_dir) + 1;
	if (dir == NULL || *len == 0) {
		*len = dir_len;
		return 0;
	}
	size_t min_len = dir_len > *len ? *len : dir_len;
	strncpy(dir, i_dir, min_len-1);
	dir[min_len-1] = '\0';
	return 0;
}

int INRSC_gethtml(INRSC *irsc, char *html, size_t *len) {
	if (!INRSC_ishtml(irsc)) return 1;
	
	char *i_html = irsc->rsc->hrsc->html;
	size_t html_len = strlen(i_html) + 1;
	if (!html || !(*len)) {
		*len = html_len;
		return 0;
	}
	size_t min_len = html_len > *len ? *len : html_len;
	strncpy(html, i_html, min_len-1);
	html[min_len-1] = '\0';
	return 0;
}

int INRSC_getheader(INRSC *irsc, char *header, size_t *len) {
	if (!irsc) return 1;
	char *i_header = irsc->header;
	size_t header_len = strlen(i_header) + 1;
	if (!header || !(*len)) {
		*len = header_len;
		return 0;
	}
	size_t min_len = header_len > *len ? *len : header_len;
	strncpy(header, i_header, min_len-1);
	header[min_len-1] = '\0';
	return 0;
}

int INRSC_setfilename(INRSC *irsc, char *filename) {
	if(!INRSC_isfile(irsc)) return 1;
	FILE_RSC *frsc = irsc->rsc->frsc;
	size_t filename_len = strlen(filename) + 1;
	if (frsc->filename_len < filename_len) {
		frsc->filename = (char *) realloc(frsc->filename,
						filename_len * 2);
		frsc->filename_len = filename_len * 2; 
	}
	strcpy(frsc->filename, filename);	
	return 0;
}

int INRSC_setdir(INRSC *irsc, char *dir) {
	if(!INRSC_isfile(irsc)) return 1;
	FILE_RSC *frsc = irsc->rsc->frsc;
	size_t dir_len = strlen(dir) + 1;
	if (frsc->dir_len < dir_len) {
		frsc->dir = (char *) realloc(frsc->dir,
						dir_len * 2);
		frsc->dir_len = dir_len * 2; 
	}
	strcpy(frsc->dir, dir);	
	return 0;
}

int INRSC_sethtml(INRSC *irsc, char *html) {
	if(!INRSC_ishtml(irsc)) return 1;
	HTML_RSC *hrsc = irsc->rsc->hrsc;
	size_t html_len = strlen(html) + 1;
	if (hrsc->html_len < html_len) {
		hrsc->html = (char *) realloc(hrsc->html,
						html_len * 2);
		hrsc->html_len = html_len * 2; 
	}
	strcpy(hrsc->html, html);	
	return 0;
}

int INRSC_setheader(INRSC *irsc, char *header) {
	if(!irsc) return 1;
	size_t header_len = strlen(irsc->header) + 1;
	if (irsc->header_len < header_len) {
		irsc->header = (char *) realloc(irsc->header,
						header_len * 2);
		irsc->header_len = header_len * 2; 
	}
	strcpy(irsc->header, header);	
	return 0;
}

int INRSC_settypefile(INRSC *irsc) {
	if (!irsc) return 1;
	if (INRSC_isfile(irsc)) return 0;
	if (INRSC_ishtml(irsc)) _freehtml(irsc);
	_allocfile(irsc);
	return 0;
}

int INRSC_settypehtml(INRSC *irsc) {
	if (!irsc) return 1;
	if (INRSC_ishtml(irsc)) return 0;
	if (INRSC_isfile(irsc)) _freefile(irsc);
	_allochtml(irsc);
	return 0;
}

int INRSC setfiletype(INRSC *irsc, FILE_TYPE file_type) {
	if (!irsc) return 1;
	if (!INRSC_isfile(irsc)) return 1; 
	irsc->rsc->frsc->ft = file_type;
	return 0;
}
