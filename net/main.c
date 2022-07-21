#include "html.h"
#include "inetapp.h"

int main(int argc, char *argv[]) {

	char lib[256]; 
	char *html = (char *) malloc (100000);
	char *header = (char *) malloc (1024);
	int blen = atoi(argv[2]);
	int tlen = blen;
	char links[blen][512];
	char link[1][512];

	snprintf(lib, 256, "http://libgen.rs/search.php?req=%s", argv[1]);
	get(lib, header, sizeof(header), html, sizeof(html));
	linkparse(html, "book", links, &tlen, LPBGN);

	int i;
	int tmp;
	for (int i = 0; i < tlen; i++) {
		snprintf(lib, 256, "https://libgen.rs/%s", links[i]); 
		get(lib, header, sizeof(header), html, sizeof(html));
		tmp = 1;
		linkparse(html, "library.lol", link, &tmp, 0);

		get(link[0], header, sizeof(header), html, sizeof(html));
		fprintf(stderr, "%s\n%s\n", header, html);
		tmp = 1;
		linkparse(html, "main", link, &tmp, 0);

		get(link[0], header, sizeof(header), html, sizeof(html));
	}

	free(html);
	free(header);
}
