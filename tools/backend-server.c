//
// Created by clanzy on 07.12.22.
//
#include "backend.h"
#include "constants.h"
#include "request.h"

char buffer[BUFFER_LENGTH];

int init_server(const char *addr, const char *port)
{
	int rc;

	/* Put data in hints and generate result */
	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	// hints.ai_flags = AI_PASSIVE;

	struct addrinfo *result;
	rc = getaddrinfo(addr, port, &hints, &result);
	if (rc != 0) {
		perror("getaddrinfo()");
		return -1;
	}

	/* Create & bind socket part */
	int sockfd;
	struct addrinfo *curr;
	for (curr = result; curr != NULL; curr = curr->ai_next) {
		sockfd = socket(curr->ai_family, curr->ai_socktype,
				curr->ai_protocol);
		if (sockfd < 0) {
			continue;
		}

		rc = bind(sockfd, curr->ai_addr, curr->ai_addrlen);
		if (rc == 0) {
			/* Success */
			break;
		}
		/* Close after failed bind() */
		close(sockfd);
	}
	freeaddrinfo(result);

	if (curr == NULL) {
		perror("bind()");
		return -1;
	}

	return sockfd;
}

void handle_connection(int sockfd)
{
	recv_data(sockfd);
	enum Operations op = decode_first_byte(buffer);
	switch (op) {
	case LIST:
		list_files();
	case DELETE:
		delete_file(char *fname);
	case UPLOAD:
		upload_file();
	case DOWNLOAD:
		download_file(sockfd);
	default:
		(void)fprintf(stderr, "Wrong operation decoded\n");
	}
}
