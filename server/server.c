#include "backend.h"
#include <stdio.h>
#include <pthread.h>

#define MAX_CONNECTIONS 10



int main()
{
	int sockfd = init_server("127.0.0.1", "7777");

	if (listen(sockfd, MAX_CONNECTIONS) == -1) {
		perror("listen()");
		return -1;
	}
	for (;;) {
		int connfd;
		struct sockaddr_storage connaddr;
		socklen_t len = sizeof connaddr;
		connfd = accept(sockfd, (struct sockaddr *)&connaddr, &len);
		if (connfd <= 0) {
			perror("accept()");
			return -1;
		}

		close(connfd);
	}
	return 0;
}