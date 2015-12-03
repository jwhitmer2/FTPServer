// The client, as an argument, takes the IP of a server and attempts
// to connect to it.  

// To run this program, a server program must already have been
// launched.  To compile, do:

// gcc client.c -o client.x

// To run, do:

// ./client.x IP-of-the server.

// If the server is running on the local machine, you can do:

// ./client.x 127.0.0.1

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
#include <vector>


#define PORT_NUMBER temp
#define MAXLINE 200


bool cache = false;

int temp;

std::vector<std::string> cachedClients;


void processRequest(int sockfd)
{// Function to handle incoming requests
	char *incomingMsg;
	incomingMsg = (char *)malloc(sizeof(char) * MAXLINE);
	
	while (1)
	{
		int readBytes = read(sockfd, incomingMsg, sizeof(char) * MAXLINE);
		
		if (strncmp(incomingMsg, "cache", strlen(incomingMsg) - 1) == 0)
		{// Server has notified client that it will become a cache server.
			cache = true;
			std::cout << "\nClient is now a cache server...\n";
			// Recieve the addresses of clients to be cached
			// First send number of clients that will be cached
			// Then call read N number of times, adding each address to vector
			char *numClients = (char *)malloc(sizeof(char) * MAXLINE);
			readBytes = read(sockfd, numClients, sizeof(char) * MAXLINE);
			int n = atoi(numClients);	
			std::cout << "Caching " << n << " clients.\n";
		
			char *buf = (char *)malloc(sizeof(char) * MAXLINE);	

			for (int i = 0; i < n; i++)
			{// Receive all the addresses of the clients to be cached
				memset(buf, '\0', MAXLINE);
				readBytes = read(sockfd, buf, sizeof(char) * MAXLINE);
				std::string cacheClient = buf;
				cachedClients.push_back(cacheClient);							

			}
			free(numClients);
			free(buf);
		}

		memset(incomingMsg, '\0', MAXLINE);
	}
}

int main(int argc, char **argv)
{
	std::vector<std::thread> threadVect;	

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

	threadVect.push_back(std::thread(processRequest, sockfd));

	while (1)
	{
		char *buffer;
		buffer = (char *)malloc(MAXLINE * sizeof(char));
		
	
		printf("Enter command: ");

		size_t numRead = getline(&buffer, &MAX, stdin);
	
		// First send length of command to server
		if (write(sockfd, &numRead, sizeof(numRead)) < 0)
		{
			std::cout << "Error \n";
			exit(0);
		}

	//	std::cout << numRead << '\n';
	
		// Then write the actual command to server
	
		write(sockfd, buffer, sizeof(char) * numRead);
		if (strncmp(buffer, "shutdown", strlen(buffer)-1) == 0)
			break;
		memset(buffer, '\0', numRead);	
			
	}
	for (int i = 0; i < threadVect.size(); i++)
	{
		threadVect[i].join();
	}
	printf("Ending connection with server.\n");
	close( sockfd );
	exit(0);
}
