#include "backend.h"
#include "utilities.h"
#include "request.h"
#include "constants.h"
#include <dirent.h>

extern char buffer[BUFFER_LENGTH];

void drop_first_byte(int sockfd)
{
	recv(sockfd, buffer, 1, 0);
}

int download_loop(uint64_t filesize, int fd, int sock)
{
	char *bufferptr = buffer;
	uint64_t stored_filesize = filesize;
	while (filesize > 0) {
		int length = min(BUFFER_LENGTH, filesize);
		filesize -= length;
		char *current = bufferptr;
		while (length > 0) {
			int rc = recv(sock, current, length, MSG_WAITALL);
			if (rc <= 0) {
				perror("recv()");
				close(fd);
				return -1;
			}
			length -= rc;
			current += rc;
		}
		if (write(fd, bufferptr,
			  min(BUFFER_LENGTH - length, stored_filesize)) == -1) {
			perror("write()");
			close(fd);
			return -1;
		}
	}
	return 0;
}

int download_file(int sockfd)
{
	if (recv(sockfd, buffer, g_upload_download_header_length,
		 MSG_WAITALL) <= 0) {
		perror("recv()");
		return -1;
	}

	static char filename[FILENAME_LENGTH];

	uint64_t filesize;
	decode_download(buffer, filename, &filesize);

	printf("Downloading file %s (%ld bytes)\n", filename, filesize);
	int fd = open(filename, O_CREAT | O_WRONLY, PERMISSIONS);
	if (fd == -1) {
		perror("open()");
		return -1;
	}

	if (download_loop(filesize, fd, sockfd) == -1) {
		perror("download_loop()");
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}

int send_loop(char *bufferptr, int bytes_used, int fd, int sock)
{
	int rc;
	int bytes_read;
	while ((bytes_read = read(fd, bufferptr + bytes_used,
				  BUFFER_LENGTH - bytes_used)) != 0) {
		/* Check if read successful */
		if (bytes_read == -1) {
			perror("read()");
			close(fd);
			return -1;
		}

		int length = bytes_read + bytes_used;
		char *current = bufferptr;
		bytes_used = 0;
		while (length > 0) {
			rc = send(sock, current, length, 0);
			//printf("bytes sent %d\n", rc);
			if (rc == -1) {
				perror("send()");
				close(fd);
				return -1;
			}
			if (rc == 0) {
				perror("send()");
				close(fd);
				return -1;
			}
			current += rc;
			length -= rc;
		}
	}
	return 0;
}

int send_file(int sockfd, char *fpath, int bytes_used)
{
	char *bufferptr = buffer;
	/* Storing in bytes_used how many bytes were used by header */
	int fd = open(fpath, O_RDONLY);
	if (fd == -1) {
		perror("open()");
		return -1;
	}
	printf("Sending file %s\n", fpath);
	if (send_loop(bufferptr, bytes_used + 1, fd, sockfd) == -1) {
		perror("send_loop()");
		close(fd);
		return -1;
	}

	return 0;
}
