#include <stdio.h>
#include "client-interface.h"
#include "backend.h"
#include "utilities.h"

int start_client(const char *addr, const char *port)
{
	int sockfd = init_client(addr, port);
	if (sockfd == -1) {
		perror("init_client()");
		return -1;
	}
	//printf("Enter your username: ");
	//TODO username input
	return sockfd;
}

void print_help()
{
	static char *help =
		"Commands:\n"
		"\tlist - prints the names of all files that are stored on the server.\n"
		"\tdownload <filename> [<filename> ...] - downloads one or more from the server to a current directory.\n"
		"\tupload <path_to_file> [<path_to_file> ...] - uploads one or more files to the sever.\n"
		"\tdelete <filename> [<filename> ...] - deletes one or more files from the server.\n"
		"\texit - exit the program\n";
	printf("%s", help);
}

void list_files(int sockfd)
{
	request_list(sockfd);
	//server sends filenames
	while (procceed_list(sockfd) == 1) {
	}
}

void delete_files(int sockfd, char *arguments)
{
	request_delete(sockfd, arguments);
	//serves deletes files and sends success
	get_responce(sockfd);
}

void upload_files(int sockfd, char *arguments)
{
	char *token;
	char *temp;
	for_each_token(token, arguments, " \f\n\r\t\v", temp) {
		request_upload(sockfd, token);
		//server downloads a file and sends success
		get_responce(sockfd);
	}
}

void download_files(int sockfd, char *arguments)
{
	char *token;
	char *temp;
	for_each_token(token, arguments, " \f\n\r\t\v", temp) {
		request_download(sockfd, token);
		//server sends a file
		drop_first_byte(sockfd);
		download_file(sockfd);
	}
}
