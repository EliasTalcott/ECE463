#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include<sys/select.h>

#define LISTENQ 10
#define MAXLINE 1024

int max(int a, int b) {
	if (a >= b) {
		return a;
	} else {
		return b;
	}
}

int open_listenfd_tcp(int port)  
{ 
  	int listenfd, optval=1; 
  	struct sockaddr_in serveraddr; 
   
  	// Create a socket descriptor
  	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    		return -1; 
  
  	// Eliminates "Address already in use" error from bind.
  	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,  (const void *)&optval , sizeof(int)) < 0) 
    		return -1; 
 
  	// Listenfd will be an endpoint for all requests to port on any IP address for this host
  	bzero((char *) &serveraddr, sizeof(serveraddr)); 
  	serveraddr.sin_family = AF_INET;  
  	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);  
  	serveraddr.sin_port = htons((unsigned short)port);  
  	if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) 
    		return -1; 
 
  	// Make it a listening socket ready to accept connection requests
  	if (listen(listenfd, LISTENQ) < 0) 
    		return -1; 
 
  	return listenfd; 
} 


int open_listenfd_udp(int port)  
{ 
	int listenfd, optval=1; 
	struct sockaddr_in serveraddr; 
   
	// Create a socket descriptor
	if ((listenfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
		return -1; 
  
	// Eliminates "Address already in use" error from bind.
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,  (const void *)&optval , sizeof(int)) < 0) 
		return -1; 
 
	// Listenfd will be an endpoint for all requests to port on any IP address for this host
	bzero((char *) &serveraddr, sizeof(serveraddr)); 
	serveraddr.sin_family = AF_INET;  
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);  
	serveraddr.sin_port = htons((unsigned short)port);  
	if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) 
		return -1; 
 
	return listenfd; 
}


// Write the request from the client into a buffer
void get_request(int connfd, char * req) {
	size_t n;
	// Read into buffer while it is not full
	bzero(req, sizeof(req));
	n = read(connfd, req, MAXLINE);
	write(connfd, req, n);
}


// Encrypt buffer using a Caeser Cipher
void encrypt(char * buf, int shift_number) {
	int length = strlen(buf);
	int i;
	for(i = 0; i < length; i++) {
		if (buf[i] >= 'a' && buf[i] <= 'z') {
			buf[i] = buf[i] - shift_number;
			if (buf[i] < 'a') {
				buf[i] = buf[i] + 26;
			}
		}
		if (buf[i] >= 'A' && buf[i] <= 'Z') {
			buf[i] = buf[i] - shift_number;
			if (buf[i] < 'A') {
				buf[i] = buf[i] + 26;
			}
		}
	}
}

// Execute instructions from request
void send_response(int connfd, char * req) {
	// Check if request matches "GET <filepath> <shift number> HTTP/1.0<CR><LF><CR><LF>"
	int i = 0;
	int j = 0;
	int req_length = strlen(req);

	// Initialize parts of request buffers
	char type[MAXLINE];
	char filename[MAXLINE];
	char shift_number[MAXLINE];
	char the_rest[MAXLINE];
	bzero(type, sizeof(type));
	bzero(filename, sizeof(filename));
	bzero(shift_number, sizeof(shift_number));
	bzero(the_rest, sizeof(the_rest));

	// Read request into individual buffers
	// Request type
	while (req[i] != '\0' && req[i] != ' ') {
		type[i] = req[i];
		i++;
		j++;
	}
	// Filename
	i++;
	j = 0;
	while (req[i] != '\0' && req[i] != ' ') {
		filename[j] = req[i];
		i++;
		j++;
	}
	int k = 1;
	char temp;
	char new_temp;
	int path_length = strlen(filename);
	if (filename[0] == '/') {
		temp = filename[0];
		filename[0] = '.';
		while (k <= path_length) {
		  	new_temp = filename[k];
			filename[k] = temp;
			temp = new_temp;
			k++;
		}
	}
	// Shift number
	i++;
	j = 0;
	while (req[i] != '\0' && req[i] != ' ') {
		shift_number[j] = req[i];
		i++;
		j++;
	}
	int shift_number_int = atoi(shift_number);
	// The rest
	i++;
	j = 0;
	while (req[i] != '\0' && req[i] != ' ') {
		the_rest[j] = req[i];
		i++;
		j++;
	}
	// Make sure request is of correct type
	// Only accept GET requests (includes get)
	if (!strcmp(type, "GET") &&  !strcmp(type, "get")) {
		return;
	}
	// Only accept HTTP/1.0 requests
	if (!strcmp(the_rest, "HTTP/1.0\n")) {
		return;
	}

	// Check if file at filepath exists
	char * buf;
	char * response_code;
    	buf = (char *) malloc(MAXLINE);
	response_code = (char *) malloc(MAXLINE);
	bzero(buf, MAXLINE);
	bzero(response_code, MAXLINE);
	size_t n;
	int rval = access(filename, R_OK);
	if (rval == 0) {
	  	// File is readable (200)
		response_code = "HTTP/1.0 200 OK\r\n\r\n";
		write(connfd, response_code, strlen(response_code));

		// Write file contents to socket
		FILE * fpin = fopen(filename, "r");
		if (fpin != NULL) {
			while (n = fread(buf, sizeof(char), MAXLINE, fpin) != 0) {
				encrypt(buf, shift_number_int);
				write(connfd, buf, strlen(buf));
				bzero(buf, MAXLINE);	
			}
		}
		fclose(fpin);
	}
	else {
		rval = access(filename, F_OK);
		if (rval == 0) {
			// File is not readable (403)
			response_code = "HTTP/1.0 403 Forbidden\r\n\r\n";
			write(connfd, response_code, strlen(response_code));
		}	
		else {
		  	// File not found (404)
			response_code = "HTTP/1.0 404 Not Found\r\n\r\n";
			write(connfd, response_code, strlen(response_code));
		}
	}
}


int main(int argc, char **argv) {
  	int listenfd_tcp, listenfd_udp, connfd, port_tcp, port_udp, clientlen, maxfd;
  	struct sockaddr_in clientaddr;
  	struct hostent *hp;
  	char *haddrp;	
  	char * req;
  	fd_set rfds;
	char buf[MAXLINE];
	char ping[MAXLINE];
	char hostboi[MAXLINE];
	uint32_t ack;
	char outbuf[MAXLINE];
	struct sockaddr_in pingaddr;
	struct hostent *pp;

  	// Start listening on port in command line arguments
  	port_tcp = atoi(argv[1]);
  	port_udp = atoi(argv[2]);
	// TCP listen stuff
  	listenfd_tcp = open_listenfd_tcp(port_tcp); 
	// UDP listen stuff
  	listenfd_udp = open_listenfd_udp(port_udp); 
	
	maxfd = max(listenfd_tcp, listenfd_udp) + 1;
	FD_ZERO(&rfds);
  	while (1) {
    		// Do select magic
		FD_SET(listenfd_tcp, &rfds);
		FD_SET(listenfd_udp, &rfds);
		select(maxfd, &rfds, NULL, NULL, NULL);
		
		// Do TCP stuff
		if (FD_ISSET(listenfd_tcp, &rfds)) {
			clientlen = sizeof(clientaddr); 
    			connfd = accept(listenfd_tcp, (struct sockaddr *)&clientaddr, &clientlen);
			hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
			haddrp = inet_ntoa(clientaddr.sin_addr);
			// Time to get forky
			int childpid;
			if ((childpid = fork()) == 0) {
		  		close(listenfd_tcp);

    				// Receive an HTTP request
    				req = (char *) malloc(MAXLINE);
				bzero(req, MAXLINE);
				get_request(connfd, req);
		
    				// Send an HTTP response and close the connection
				send_response(connfd, req);
				exit(0);
    			}
			close(connfd);
		}
		// Do UDP stuff
		else if (FD_ISSET(listenfd_udp, &rfds)) {
			// Receive ping from client
			clientlen = sizeof(clientaddr); 
			bzero(buf, sizeof(buf));
			recvfrom(listenfd_udp, buf, sizeof(buf), 0, (struct sockaddr*)&clientaddr, &clientlen);
			int buflen = strlen(buf);
			// Clear buffers
			bzero(ping, sizeof(ping));
			bzero(outbuf, sizeof(outbuf));
			bzero(hostboi, sizeof(hostboi));
			// Fill buffers
			memcpy(&ping, &buf[0], buflen);
			memcpy(&ack, &buf[buflen], sizeof(uint32_t));
			// Find hostname from IP
			pingaddr.sin_family = AF_INET;
			pingaddr.sin_addr.s_addr = inet_addr(ping);
			struct hostent * pp = gethostbyaddr((const char *)&pingaddr.sin_addr.s_addr, sizeof(pingaddr.sin_addr.s_addr), AF_INET);
			int pinglen = strlen(pp->h_name);
			memcpy(&hostboi, pp->h_name, pinglen);
			// Add one to sequence number
			ack = htonl(ntohl(ack) + 1);
			// Copy values into output buffer and send ACK
			memcpy(&outbuf, hostboi, pinglen);
			memcpy(&outbuf[pinglen], &ack, sizeof(uint32_t));
			sendto(listenfd_udp, (const char*)outbuf, pinglen + sizeof(unsigned int), 0, (struct sockaddr*)&clientaddr, clientlen);
		}
  	}
}

