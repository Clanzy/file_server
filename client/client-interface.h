#pragma once

int start_client(const char *addr, const char *port);

void print_help();
void list_files(int sockfd);
void download_files(int sockfd, char *arguments);
void upload_files(int sockfd, char *arguments);
void delete_files(int sockfd, char *arguments);

int close_socket();
