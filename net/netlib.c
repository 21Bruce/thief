#include "netlib.h"
#define BUF_LEN 1024

static int connect_lg();

static int connect_lg() {
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
	return lgfd;
}


FILE *search_html(char *title) {
	// Initialization of libgen socket
	int lgfd = connect_lg();	
	char search_req [BUF_LEN];

	snprintf(search_req, BUF_LEN, "GET /search.php?req=%s&lg_topic=libgen&open=0&view=simple&res=25&phrase=1&column=def HTTP/1.1\r\nhost: libgen.rs\r\n\r\n", title);
	send(lgfd, search_req, strlen(search_req), 0);

	char res_buf [BUF_LEN]; 
	ssize_t res_len;
	FILE *tmp = tmpfile();
	while ((res_len = recv(lgfd, res_buf, BUF_LEN, 0)) > 0) {
		fwrite(res_buf, 1, res_len, tmp);
		printf("%.*s\n", (int)res_len, res_buf);
	}

	return tmp;
}
