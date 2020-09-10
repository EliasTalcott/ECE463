#include<sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 1024
#define MAXREQUEST 512

int open_clientfd(char * hostname, int port);


int open_clientfd(char * hostname, int port) {
	int clientfd;
	struct hostent * hp;
	struct sockaddr_in serveraddr;

	if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}

	if ((hp = gethostbyname(hostname)) == NULL) {
	  return -2;
	}
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *) hp -> h_addr, (char *) &serveraddr.sin_addr.s_addr, hp -> h_length);
	serveraddr.sin_port = htons(port);

	// Establish a connection with the server
	if (connect(clientfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
	  return -1;
	}

	return clientfd;
}


int main(int argc, char ** argv) {
	// Declare client variables and buffers
	int clientfd, port;
	char * host;
	char buf[MAXLINE];
	char * path;
	char filename[MAXREQUEST];

	// Initialize client variables and buffers
	host = argv[1];
	port = atoi(argv[2]);
	path = argv[3];


	// Open first connection and GET filename
	clientfd = open_clientfd(host, port);
	if (clientfd < 0) {
		printf("Error opening connection\n");
		exit(0); 
	}
	// Compose and send request
	char message[MAXREQUEST];
	snprintf(message, sizeof(message), "GET %s HTTP/1.0\r\n\r\n", path);
	send(clientfd, message, strlen(message), 0);
	// Receive response, check for code 200, and send server response to stdout
	recv(clientfd, buf, MAXLINE - 1, 0);
	if (strstr(buf, "200 OK") != NULL) {
		int length = strlen(buf);
		int n = 0;
		int num_newlines = 0;
		while (num_newlines < 12) {
			if (buf[n] == '\n') {
				num_newlines++;
			}
			putchar(buf[n]);
			n++;
		}
	} else {
		printf("First GET request did not return code 200");
		return(0);
	}

	// Extract filename from server response
	int length = strlen(buf);
	int i = 0;
	int n = 1;
	char b = buf[n - 1]; 
	char c = buf[n];
	while (b != '\n' || c != '\r') {
		n++;
		b = buf[n - 1];
		c = buf[n];
	}
	n += 2;
	i = 0;
	while (buf[n] != '\n') {
		filename[i] = buf[n];
		i++;
		n++;
	}

	// Reopen connection and GET file contents
	clientfd = open_clientfd(host, port);
	if (clientfd < 0) {
		printf("Error opening connection\n");
		exit(0); 
	}
	// Compose and send request
	bzero(message, MAXREQUEST);
	snprintf(message, MAXREQUEST, "GET %s HTTP/1.0\r\n\r\n", filename);
	send(clientfd, message, strlen(message), 0);
	// Receive and send server response to stdout in chunks of size MAXLINE
	while (recv(clientfd, buf, MAXLINE - 1, 0) != 0) {
		puts(buf);
		bzero(buf, MAXLINE);
	}
}
