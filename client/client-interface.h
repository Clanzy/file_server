//
// Created by clanzy on 04.12.22.
//

#ifndef FILE_SERVER_CLIENT_INTERFACE_H
#define FILE_SERVER_CLIENT_INTERFACE_H

int start_client(const char *addr, const char *port);

void print_help();
void list_files(int sockfd);
void download_files(int sockfd, char *arguments);
void upload_files(int sockfd, char *arguments);
void delete_files(int sockfd, char *arguments);

int close_socket();


#endif //FILE_SERVER_CLIENT_INTERFACE_H
