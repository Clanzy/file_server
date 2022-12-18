#ifndef FILE_SERVER_BACKEND_H
#define FILE_SERVER_BACKEND_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>


// common functions

int send_file(int sockfd, char *fpath, int bytes_used);
int download_file(int sockfd);


// client side

int init_client(const char *addr, const char *port);

int request_list(int sockfd);
int request_delete(int sockfd, char *arguments);
int request_upload(int sockfd, char *fpath);
int request_download(int sockfd, char *fname);

int procceed_list(int sockfd);

int get_responce(int sockfd);

// server side

int init_server(const char *addr, const char *port);

#endif
