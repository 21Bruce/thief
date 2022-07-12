#include "thief.h"
#define BUFLEN 1024

int main (int argc, char *argv[]) {

	// Initialization of libgen socket
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo *lg_addr;
	if (getaddrinfo("libgen.rs", "80", &hints, &lg_addr))
		fatal();
	int lgfd = socket(lg_addr->ai_family,
			lg_addr->ai_socktype,
			lg_addr->ai_protocol);
	if (lgfd < 0) fatal();
	if (connect(lgfd, lg_addr->ai_addr, lg_addr->ai_addrlen))
		fatal();
	freeaddrinfo(lg_addr);

	char search_req [BUFLEN];

	snprintf(search_req, BUFLEN, "GET /search.php?req=%s&lg_topic=libgen&open=0&view=simple&res=25&phrase=1&column=def HTTP/1.1\nhost: libgen.rs\n\r\n", argv[1]);
	send(lgfd, search_req, strlen(search_req), 0);

	char res_buf [BUFLEN]; 
	int res_len;
	while ((res_len = recv(lgfd, res_buf, BUFLEN, 0)) > 0)
		printf("%.*s", res_len, res_buf);

	return 0;
}

