//
// Created by clanzy on 05.12.22.
//

#ifndef FILE_SERVER_REQUEST_H
#define FILE_SERVER_REQUEST_H

#include <stdbool.h>


// compiler magic to make enum 1 byte on all architectures
enum __attribute__((__packed__)) Operations {

	LIST,
	DELETE,
	UPLOAD,
	DOWNLOAD,

	MAX_OPERATIONS
};

void encode_list(char *buffer);
void encode_delete(char *buffer, char *args);
int encode_upload(char *fpath, char *buffer, int *bytes_used);
void encode_download(char *fpath, char *buffer);

enum Operations decode_first_byte(char *buffer);
void decode_download(char *buffer, char *fname, uint64_t *filesize, int *bytes_read);

#endif //FILE_SERVER_REQUEST_H