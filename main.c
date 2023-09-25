#include "./net/html.h"
#include "./net/inetapp.h"

int main(int argc, char *argv[]) {

	int batch = 8;
	char query[512]; 
	char *html = (char *) malloc (100000);
	char *header = (char *) malloc (1024);
	int blen = atoi(argv[2]);
	int tlen = blen;
	char links[blen][512];
	char link[1][512];

	snprintf(query, sizeof(query), "http://libgen.rs/search.php?req=%s", argv[1]);
	get(query, header, sizeof(header), html, sizeof(html));
	linkparse(html, "book", links, &tlen, LPBGN);

	int tmp;
	pid_t pid;
	int i;
	int j;
	for (i = 0; i < tlen; i++) {
		if ((pid = fork()) == 0) {
			snprintf(query, sizeof(query), "http://libgen.rs/%s", links[i]); 
			get(query, header, sizeof(header), html, sizeof(html));
			tmp = 1;
			linkparse(html, "library.lol", link, &tmp, 0);

			get(link[0], header, sizeof(header), html, sizeof(html));
			tmp = 1;
			linkparse(html, "main", link, &tmp, 0);

			get(link[0], header, sizeof(header), html, sizeof(html));
			fprintf(stderr, "Processing Book: %d\n", i);
			sleep(60);
			break;
		} else {
			if ((i + 1) % batch == 0) { 
                printf("Batching...\n");
				for (j = 0; j < batch; j++)
					wait(NULL);
			}
		}

	}

	if (pid != 0)
		for (i = 0; i < tlen % batch; i++)
			wait(NULL);

	free(html);
	free(header);
}
