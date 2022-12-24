#pragma once

#include <stdbool.h>
#include <stdint.h>

// compiler magic to make enum 1 byte on all architectures
enum __attribute__((__packed__)) Operations {

	LIST,
	DELETE,
	UPLOAD,
	DOWNLOAD,

	SUCCESS,

	MAX_OPERATIONS
};

extern int g_upload_download_header_length;

void encode_list(char *buffer);
void encode_delete(char *buffer, char *args);
int encode_upload(char *fpath, char *buffer);
void encode_download(char *fpath, char *buffer);

void encode_responce(char *buffer);

enum Operations decode_first_byte(char *buffer);
void decode_download(char *buffer, char *fname, uint64_t *filesize);
