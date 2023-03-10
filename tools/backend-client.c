#include "backend.h"
#include "constants.h"
#include "request.h"
#include "utilities.h"

#include <string.h>

char buffer[BUFFER_LENGTH];

int request_list(int sockfd)
{
	encode_list(buffer);
	if (send(sockfd, buffer, 1, 0) <= 0) {
		perror("send()");
		return -1;
	}
	return 0;
}

int procceed_fnames_buffer(char *buff)
{
	char *token;
	char *temp;
	for_each_token(token, buff, " \f\n\r\t\v", temp) {
		if (strcmp(token, "/") == 0) {
			return 0;
		}
		printf("%s\n", token);
	}
	return 1;
}

int procceed_list(int sockfd)
{
	int rc = recv(sockfd, buffer, BUFFER_LENGTH, MSG_WAITALL);
	if (rc <= 0) {
		perror("recv()");
		return -1;
	}

	if (procceed_fnames_buffer(buffer) == 0) {
		/* Returns success code */
		return 0;
	}
	/* Return code that means there is more files to come */
	return 1;
}

int request_delete(int sockfd, char *arguments)
{
	encode_delete(buffer, arguments);
	if (send(sockfd, buffer, BUFFER_LENGTH, 0) <= 0) {
		perror("send()");
		return -1;
	}
	return 0;
}

int request_upload(int sockfd, char *fpath)
{
	int bytes_used;

	if (encode_upload(fpath, buffer) == -1) {
		perror("encode_upload()");
		return -1;
	}
	if (send_file(sockfd, fpath, g_upload_download_header_length) == -1) {
		perror("send_file()");
		return -1;
	}

	return 0;
}

int request_download(int sockfd, char *fname)
{
	encode_download(fname, buffer);
	if (send(sockfd, buffer, BUFFER_LENGTH, 0) <= 0) {
		perror("send()");
		return -1;
	}
	return 0;
}

int init_client(const char *addr, const char *port)
{
	int rc;

	/* Put data in hints and generate result */
	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo *result;
	rc = getaddrinfo(addr, port, &hints, &result);
	if (rc != 0) {
		perror("getaddrinfo()");
		return -1;
	}

	/* Create & connect socket part */
	int sockfd;
	struct addrinfo *curr;
	for (curr = result; curr != NULL; curr = curr->ai_next) {
		sockfd = socket(curr->ai_family, curr->ai_socktype,
				curr->ai_protocol);
		if (sockfd < 0) {
			continue;
		}

		rc = connect(sockfd, curr->ai_addr, curr->ai_addrlen);
		if (rc == 0) {
			/* Success */
			break;
		}
		/* Close after failed connect() */
		close(sockfd);
	}

	freeaddrinfo(result);

	if (curr == NULL) {
		perror("connect()");
		return -1;
	}

	return sockfd;
}

int get_responce(int sockfd)
{
	if (recv(sockfd, buffer, BUFFER_LENGTH, MSG_WAITALL) <= 0) {
		perror("recv()");
		return -1;
	}
	return 0;
}
