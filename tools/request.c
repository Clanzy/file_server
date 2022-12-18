//
// Created by clanzy on 05.12.22.
//

#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include "request.h"
#include "utilities.h"
#include "constants.h"

void encode_list(char *buffer)
{
	enum Operations temp = LIST;
	memcpy(buffer, &temp, sizeof temp);
}

void encode_delete(char *buffer, char *args)
{
	enum Operations temp = DELETE;
	memcpy(buffer, &temp, sizeof temp);

	buffer[sizeof temp] = '\0';
	char *token;
	char *tmp;
	for_each_token(token, args, " ", tmp) {
		strcat(buffer, token);
	}
}

int encode_upload(char *fpath, char *buffer, int *bytes_used)
{
	enum Operations temp = UPLOAD;
	memcpy(buffer, &temp, sizeof temp);
	buffer = buffer + sizeof temp;

	/* Get and encode file size as signed 64-bit integer */
	struct stat info;
	if (stat(fpath, &info) == -1) {
		perror("stat()");
		return -1;
	}
	uint64_t size = info.st_size;
	size = htonll(size);
	memcpy(buffer, &size, sizeof size);
	buffer = buffer + sizeof size;

	/* Get name of the file and encode as next FILENAME_LENGTH bytes */
	size_t len = strlen(fpath) + 1;
	int fname_len;
	char *fname = rindex(fpath, '/');
	if (fname == NULL) {
		fname_len = len;
		fname = fpath;
	} else {
		++fname;
		fname_len = strlen(fname) + 1;
	}
	if (fname_len > FILENAME_LENGTH) {
		(void)fprintf(
			stderr,
			"filename length must be shorter than %d symbols\n",
			FILENAME_LENGTH);
		return -1;
	}
	memcpy(buffer, fname, fname_len);
	*bytes_used = sizeof temp + sizeof size + FILENAME_LENGTH;

	return 0;
}

void encode_download(char *fpath, char *buffer)
{
	enum Operations temp = DOWNLOAD;
	memcpy(buffer, &temp, sizeof temp);
	buffer = buffer + sizeof temp;

	size_t len = strlen(fpath) + 1;
	memcpy(buffer, fpath, len);
}

enum Operations decode_first_byte(char *buffer)
{
	enum Operations temp;
	memcpy(&temp, buffer, sizeof temp);
	return temp;
}

void decode_download(char *buffer, char *fname, uint64_t *filesize,
		    int *bytes_read)
{
	memcpy(filesize, buffer, sizeof *filesize);
	*filesize = ntohll(*filesize);
	buffer += sizeof *filesize;

	if (fname) {
		memcpy(fname, buffer, FILENAME_LENGTH);
	}

	*bytes_read = sizeof *filesize + FILENAME_LENGTH;
}