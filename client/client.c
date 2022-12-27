#include "client-interface.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PORT "7778"
#define ADDRESS "127.0.0.1"

int main()
{
#define BUFFER_SIZE 2048
	int sockfd = start_client(ADDRESS, PORT);
	if (sockfd == -1) {
		perror("start_client()");
		return -1;
	}
	printf("You have successfully connected to the file server.\n");
	printf("Type 'help' to list all available commands.\n");

	char buffer[BUFFER_SIZE];
	for (;;) {
		if (fgets(buffer, sizeof buffer, stdin)) {
			char *command;
			char *arguments;
			command = strtok_r(buffer, " \f\n\r\t\v", &arguments);
			if (strcmp(command, "help") == 0) {
				print_help();
			} else if (strcmp(command, "list") == 0) {
				list_files(sockfd);
			} else if (strcmp(command, "download") == 0) {
				download_files(sockfd, arguments);
			} else if (strcmp(command, "upload") == 0) {
				upload_files(sockfd, arguments);
			} else if (strcmp(command, "delete") == 0) {
				delete_files(sockfd, arguments);
			} else if (strcmp(command, "exit") == 0) {
				close(sockfd);
				break;
			} else {
				printf("Bad input. Type 'help' to list all available commands.\n");
			}
		}
	}
	close(sockfd);
	return 0;
}
