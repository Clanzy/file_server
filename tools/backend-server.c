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
	static DIR *d;
	static struct dirent *dir;
	int used = 1;
	buff[0] = '\0';

	if (d == NULL) {
		d = opendir(".");
	} else if (dir != NULL) {
		int filelen = strlen(dir->d_name);
		(void)sprintf(buff, "%s %s", buff, dir->d_name);
		used += filelen;
	}

	if (d) {
		while ((dir = readdir(d)) != NULL) {
			int filelen = strlen(dir->d_name);
			if (used + filelen + 1 > FILENAME_LENGTH) {
				return 1;
			}

			(void)sprintf(buff, "%s %s", buff, dir->d_name);
			used += filelen;
		}

		dir = NULL;

		if (used + 1 + 1 > FILENAME_LENGTH) {
			return 1;
		}

		(void)sprintf(buff, "%s /", buff);

		closedir(d);
		d = NULL;
		return 0;
	}
	return -1;
}

int send_filenames(int sockfd)
{
	while (write_fnames_buffer(buffer) == 1) {
		if (send(sockfd, buffer, BUFFER_LENGTH, 0) <= 0) {
			perror("send()");
			return -1;
		}
	}
	if (send(sockfd, buffer, BUFFER_LENGTH, 0) <= 0) {
		perror("send()");
		return -1;
	}
	printf("Sent filenames\n");
	return 0;
}

int delete_files(int sockfd, char *buffer)
{
	if (recv(sockfd, buffer, BUFFER_LENGTH-1, 0) <= 0) {
		perror("recv()");
		return -1;
	}
	char *token;
	char *temp;
	for_each_token(token, buffer, " \n\t", temp) {
		printf("Deleted file: %s\n", token);
		if (remove(token) == -1) {
			perror("remove()");
		}
	}
	return 0;
}

int send_responce(int sockfd, char *buffer)
{
	encode_responce(buffer);
	if (send(sockfd, buffer, BUFFER_LENGTH, 0) <= 0) {
		perror("send()");
		return -1;
	}
	return 0;
}

int get_fname(int sockfd, char *buffer, char *fname)
{
	if (recv(sockfd, buffer, BUFFER_LENGTH - 1, MSG_WAITALL) <= 0) {
		perror("recv()");
		return -1;
	}
	memcpy(fname, buffer, FILENAME_LENGTH);
	return 0;
}

void handle_connection(int sockfd)
{
	for (;;) {
		memset(buffer, 0, BUFFER_LENGTH);
		if (recv(sockfd, buffer, 1, MSG_WAITALL) <= 0) {
			perror("recv()");
			return;
		}
		enum Operations op = decode_first_byte(buffer);

		static char fname[FILENAME_LENGTH];
		switch (op) {
		case LIST:
			if (recv(sockfd, buffer, BUFFER_LENGTH-1, MSG_WAITALL) <= 0) {
				perror("recv()");
				return;
			}
			if (send_filenames(sockfd) == -1) {
				perror("send_filenames()");
				return;
			}
			break;

		case DELETE:
			if (delete_files(sockfd, buffer) == -1) {
				perror("delete_files()");
				return;
			}
			if (send_responce(sockfd, buffer) == -1) {
				perror("send_responce()");
				return;
			}
			break;

		case UPLOAD:
			if (download_file(sockfd) == -1) {
				perror("download_file()");
				return;
			}
			if (send_responce(sockfd, buffer) == -1) {
				perror("send_responce()");
				return;
			}
			break;

		case DOWNLOAD:
			if (get_fname(sockfd, buffer, fname) == -1) {
				perror("get_fname()");
				return;
			}

			if (encode_upload(fname, buffer) == -1) {
				perror("encode_upload()");
				return;
			}
			if (send_file(sockfd, fname, g_upload_download_header_length) == -1) {
				perror("send_file()");
				return;
			}
			break;

		default:
			(void)fprintf(stderr, "Wrong operation decoded\n");
		}
	}
}
