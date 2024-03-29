#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

int main(int argc, char **argv) {

	if (argc != 3) {
		perror("<Server Address> <Server Port>");
		exit(-1);
	}
	
	char *servIP = argv[1];
	
	// Set port number as given by user or as default 12345
	// in_port_t servPort = (argc == 3) ? atoi(argv[2]) : 12345;
	
	// Set port number as user specifies
	in_port_t servPort = atoi(argv[2]);
	
	//Creat a socket
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0) {
		perror("socket() failed");
		exit(-1);
	}
	
	// Set the server address
	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	int err = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
	if (err <= 0) {
		perror("inet_pton() failed");
		exit(-1);
	}
	servAddr.sin_port = htons(servPort);
	
	// Connect to server
	if (connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
		perror("connect() failed");
		exit(-1);
	}
	
	// Loopaction
    while(1) {
        printf("MYSQL: ");
        char query[BUFSIZE];
        memset(query, 0, BUFSIZE);
        fgets(query, BUFSIZE, stdin);

        if (strncmp(query, "exit", 4) == 0) {
            break;
        }

        size_t queryLen = strlen(query);
        query[queryLen-1] = '\0';

        // Send string to server
        ssize_t sentLen = send(sockfd, query, queryLen, 0);
        if (sentLen < 0) {
            perror("send() failed");
            exit(-1);
        } else if (sentLen != queryLen) {
            perror("send(): sent unexpected number of bytes");
            exit(-1);
        }

        // Receive result from server
        char buffer[BUFSIZE];
        memset(buffer, 0, BUFSIZE);
        ssize_t recvLen = recv(sockfd, buffer, BUFSIZE - 1, 0);
        if (recvLen < 0) {
            perror("recv() failed");
            exit(-1);
        } else if (recvLen == 0) {
            perror("recv() connection closed prematurely");
            exit(-1);
        }

        buffer[recvLen] = '\n';
        fputs(buffer, stdout);	
	}

	close(sockfd);
	exit(0);
}
