#include "html.h"

void linkparse(char *html, char *token, char links[][256], int *l_len, int flag) {
	char lbuf[500];	
	int clen = 0;
	int count = 0;
	char *lstrt = html;
	char *lend = html;
	char *pos;
	char delim;
	while((lstrt = strstr(lend, "href="))) { 
		lstrt += 5;	
		delim = *lstrt;	
		lstrt++;
		lend = strchr(lstrt, delim);
		int i; 
		for (i = 0; lstrt != lend; i++) 
			lbuf[i] = *lstrt++;
		lbuf[i] = '\0';	
		if ((pos = strstr(lbuf, token))) {
			if ((LPBGN & flag) && pos != lbuf) continue;
			if(clen == *l_len) return;
			strcpy(links[clen], lbuf); 
			clen++;
		}
			
	}
	*l_len = clen;
}
