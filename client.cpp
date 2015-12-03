#include<sys/types.h>	
#include<sys/socket.h>	/* basic socket definitions */
#include<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include<arpa/inet.h>

#include<sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>

#include <thread>
#include <iostream>
#include <cstdlib>

#define PORT_NUMBER temp
#define MAXLINE 200


int temp;
// The client, as an argument, takes the IP of a server and attempts
// to connect to it.  

// To run this program, a server program must already have been
// launched.  To compile, do:

// gcc client.c -o client.x

// To run, do:

// ./client.x IP-of-the server.

// If the server is running on the local machine, you can do:

// ./client.x 127.0.0.1

int main(int argc, char **argv)
{
        int sockfd, n;
	char recvline[MAXLINE + 1];
	char buf[MAXLINE + 1];
//	char buff[MAXLINE + 1];
	struct sockaddr_in	servaddr;
	size_t MAX = (size_t)MAXLINE;	

	if (argc != 3) {
	    fprintf( stderr, "Usage: %s <IPaddress>\n", argv[0] );
	    exit( 1 );
	}

	temp = atoi(argv[2]);


	// Create a socket end-point for communication.  We use the
	// IPv4 format and a stream socket. 
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    fprintf( stderr, "Socket error.  %s\n", strerror( errno ) );
	    exit( 2 );
	}

	// build a profile for the server to whom we are going to
	// communicate.  
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(PORT_NUMBER);
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
	    fprintf( stderr, "inet_pton error for %s\n", argv[1] );
	    exit( 3 );
	}

	// Attempt to connect to the server.
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
	    fprintf( stderr, "connect error: %s\n", strerror( errno ) );
	    exit( 4 );
	}

	while (1)
	{
		char *buffer;
		buffer = (char *)malloc(MAXLINE * sizeof(char));

		printf("Enter command: ");
		buffer = (char *)malloc(MAXLINE * sizeof(char));
		size_t numRead = getline(&buffer, &MAX, stdin);
	
		// First send length of command to server
		if (write(sockfd, &numRead, sizeof(numRead)) < 0)
		{
			std::cout << "Error \n";
			exit(0);
		}

		std::cout << numRead << '\n';
	
		// Then write the actual command to server
	
		write(sockfd, buffer, sizeof(char) * numRead);
		if (strncmp(buffer, "shutdown", strlen(buffer)-1) == 0)
			break;
		memset(buffer, '\0', numRead);
		
	}
	printf("Ending connection with server.\n");
	close( sockfd );
	exit(0);
}
