//
// Created by clanzy on 07.12.22.
//
#include "backend.h"
#include "constants.h"
#include "request.h"
#include "utilities.h"

#include <dirent.h>
#include <stdio.h>

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

int write_fnames_buffer(char *buff)
{
	static DIR *d = NULL;
	if (!d) {
		d = opendir(".");
	}
	static struct dirent *dir;
	int i = 1;
	buff[0] = '\0';

	static char *carryover = NULL;
	if (carryover != NULL) {
		int len = strlen(carryover);
		strncat(buff + i - 1, dir->d_name, len);
		i += len;
		carryover = NULL;
	}
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			int len = strlen(dir->d_name);
			if (BUFFER_LENGTH - i < len + 1) {
				buff[i] = '\0';
				carryover = dir->d_name;
				return 1;
			}
			strncat(buff + i - 1, dir->d_name, len);
			i += len;
			strncat(buff + i - 1, " ", 1);
			i += 1;
		}

		if (BUFFER_LENGTH - i < 2) {
			return 1;
		}
		strncat(buff + i - 1, "/", 1);
		closedir(d);
		d = NULL;
		return 0;
	}
	return -1;
}

int delete_files(char *buffer)
{
	char *token;
	char *temp;
	for_each_token(token, buffer, " \n\t", temp) {
		printf("deleted file: %s\n", token);
		if (remove(token) == -1) {
			perror("remove()");
		}
	}
	return 0;
}

void handle_connection(int sockfd)
{
	static char fname[FILENAME_LENGTH];
	for (;;) {
		memset(buffer, 0, BUFFER_LENGTH);
		if (recv(sockfd, buffer, 1, MSG_WAITALL) <= 0) {
			perror("recv()");
			return;
		}
		enum Operations op = decode_first_byte(buffer);

		int bytes_used;
		switch (op) {
		case LIST:
			while (write_fnames_buffer(buffer) == 1) {
				if (send(sockfd, buffer, BUFFER_LENGTH, 0) <=
				    0) {
					perror("send()");
					return;
				}
			}
			if (send(sockfd, buffer, BUFFER_LENGTH, 0) <= 0) {
				perror("send()");
				return;
			}
			break;

		case DELETE:
			if (recv(sockfd, buffer, BUFFER_LENGTH, 0) <=
			    0) {
				perror("recv()");
				return;
			}
			delete_files(buffer);
			encode_responce(buffer);
			if (send(sockfd, buffer, BUFFER_LENGTH, 0) <= 0) {
				perror("send()");
				return;
			}
			break;

		case UPLOAD:
			download_file(sockfd);
			encode_responce(buffer);
			if (send(sockfd, buffer, BUFFER_LENGTH, 0) <= 0) {
				perror("send()");
				return;
			}
			break;

		case DOWNLOAD:
			if (recv(sockfd, buffer, BUFFER_LENGTH, MSG_WAITALL) ==
			    -1) {
				perror("recv()");
				return;
			}
			memcpy(fname, buffer, FILENAME_LENGTH);

			if (encode_upload(fname, buffer, &bytes_used) == -1) {
				perror("encode_upload()");
				return;
			}
			if (send_file(sockfd, fname, bytes_used) == -1) {
				perror("send_file()");
				return;
			}
			break;

		default:
			(void)fprintf(stderr, "Wrong operation decoded\n");
		}
	}
}
