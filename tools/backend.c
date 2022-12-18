#include "backend.h"
#include "utilities.h"
#include "request.h"
#include "constants.h"

extern char buffer[BUFFER_LENGTH];

int download_loop(uint64_t filesize, int fd, int sock)
{
	char *bufferptr = buffer;
	while (filesize > 0) {
		int length = min(BUFFER_LENGTH, filesize);
		filesize -= length;
		char *current = bufferptr;
		while (length > 0) {
			int rc = recv(sock, current, length, MSG_WAITALL);
			if (rc == -1) {
				perror("recv()");
				close(fd);
				return -1;
			}
			if (rc == 0) {
				perror("socket probably closed");
				close(fd);
				return -1;
			}
			length -= rc;
			current += rc;
		}
		if (write(fd, bufferptr, BUFFER_LENGTH - length) == -1) {
			perror("write()");
			close(fd);
			return -1;
		}
	}
	return 0;
}

int download_file(int sockfd)
{
	if (recv(sockfd, buffer, BUFFER_LENGTH, 0) <= 0) {
		perror("recv()");
		return -1;
	}

	static char filename[FILENAME_LENGTH];

	uint64_t filesize;
	int bytes_read;
	decode_download(buffer + 1, filename, &filesize, &bytes_read);

	int fd = open(filename, O_CREAT | O_WRONLY, PERMISSIONS);
	if (fd == -1) {
		perror("open()");
		return -1;
	}

	if (write(fd, buffer + bytes_read,
		  min(BUFFER_LENGTH - bytes_read, filesize))) {
		perror("write()");
		close(fd);
		return -1;
	}
	filesize -= (BUFFER_LENGTH - bytes_read);

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

		bytes_used = 0;

		int length = bytes_read;
		char *current = bufferptr;
		while (length > 0) {
			rc = send(sock, current, length, 0);
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
	if (send_loop(bufferptr, bytes_used, fd, sockfd) == -1) {
		perror("send_loop()");
		close(fd);
		return -1;
	}

	return 0;
}

int handle_sockets(int sockfd, int max_conn)
{

}