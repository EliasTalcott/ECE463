#include<sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 100

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
	char * host, buf[MAXLINE];

	// Initialize client variables and buffers
	host = argv[1];
	port = atoi(argv[2]);

	// Open connection and GET filename
	clientfd = open_clientfd(host, port);
	if (clientfd < 0) {
		printf("Error opening connection\n");
		exit(0); 
	}
	
	// Reopen connection and GET file contents
	clientfd = open_clientfd(host, port);
	if (clientfd < 0) {
		printf("Error opening connection\n");
		exit(0); 
	}
}
