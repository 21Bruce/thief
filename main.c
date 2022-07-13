#include "netlib.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./thief title");
		exit(1);
	}
	search_html(argv[1]); 
	return 0;
}
