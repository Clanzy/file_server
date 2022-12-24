#include "backend.h"
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT "7778"
#define ADDRESS "127.0.0.1"
#define MAX_CONNECTIONS 10

void sigchld_handler(int sig)
{
	int saved_errno = errno;
	/* kills all zombies whenever a zombie appears */
	while (waitpid(-1, NULL, WNOHANG) > 0) {
	}

	errno = saved_errno;
}

int main()
{
	int sockfd = init_server(ADDRESS, PORT);

	if (listen(sockfd, MAX_CONNECTIONS) == -1) {
		perror("listen()");
		return -1;
	}

	struct sigaction sa;
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		return -1;
	}

	for (;;) {
		int connfd;
		int pid;
		struct sockaddr_storage connaddr;
		socklen_t len = sizeof connaddr;
		connfd = accept(sockfd, (struct sockaddr *)&connaddr, &len);
		if (connfd <= 0) {
			perror("accept()");
			return -1;
		}

		switch (pid = fork()) {
		case -1:
			perror("fork()");
			break;
		case 0:
			/*Child process*/
			handle_connection(connfd);
		default:
			/*Parent process*/
			break;
		}

		close(connfd);
	}
	close(sockfd);
	return 0;
}