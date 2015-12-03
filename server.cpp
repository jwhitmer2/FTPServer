#include<sys/types.h>	
#include<sys/socket.h>	
#include<arpa/inet.h>
#include<netinet/in.h>	
#include<sys/time.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>

#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <iostream>

#define MAXLINE 200
#define LISTENQ 5

#define PORT_NUMBER temp


int temp;


struct Client
{
	std::string IPAddress;
	int port;
	int connfd;				


};


std::vector<struct Client> clientConnections;
std::vector<std::string> signedClients;
std::mutex signMutex;


// This server program creates a TCP socket and listen on port "PORT_NUMBER".

// To compile enter: make
// To run, do:
// ./server.x PORT_NUMBER

// A server must be launched before clients can talk to it!


void signClient(struct Client c)
{
	std::string fullAddress = c.IPAddress + ":" + std::to_string(c.port);
	signMutex.lock();
	signedClients.push_back(fullAddress);
	signMutex.unlock();
}


void processRequest (int connfd)
{// This function determines what needs to happen next after receiving a request from a client
	printf("Processing request %d..\n", connfd);
	char buf[MAXLINE];
	int readBytes;
	while (strncmp(buf, "shutdown", strlen(buf) -1) != 0)
	{

		memset(buf, '\0', MAXLINE);
		// First receive length of message being passed
		unsigned int msgLength;
		readBytes = read(connfd, &msgLength, sizeof(msgLength));
		std::cout << msgLength;
		if (readBytes == 0)
		{
			std::cout << "uh oh\n";
			break;
		}

		// Now receive message itself

		readBytes = read(connfd, buf, msgLength * sizeof(char));

		

		if (strncmp(buf, "sign", strlen(buf) -1) != 0)
		{// Extract client IP address and port number from message sent
			std::cout << "Registering client to server...\n";
			for (int i = 0; i < clientConnections.size(); i++)
			{
				if (connfd == clientConnections[i].connfd)
				{
					std::cout << "Found client. Registering...\n";
					signClient(clientConnections[i]);
					
				}
			}		
			for (int i = 0; i < signedClients.size(); i++)
			{
				std::cout << "Signed client: " << signedClients[i] << '\n';
			}
		}
		else 
		{
			std::cout << "Failed.\n";
			std::cout << "Length of message: " << strlen(buf) << '\n';	
		}
		buf[readBytes] = '\0';
		printf("Entered in command: %s\n", buf);
//		write(connfd, buf, readBytes);
	}
	printf("Shutting server down...\n");
	close(connfd);
	exit(0);

}


void userShell(int connfd)
{
	size_t maxCommand = 20;
	char *buffer;
	buffer = (char *)malloc(20 * sizeof(char));
	while (1)
	{
		printf("Press 1 to interact with server\n");
		printf("Press 2 to interact with specified client\n");
		printf("> ");
		size_t bytesRead = getline(&buffer, &maxCommand, stdin);
		
		if (strncmp(buffer, "1", 1) == 0)
		{// Interactive server commands
			printf("Enter a command to interact with the server.\n");
			printf("Valid commands include: list, cache, and shutdown\n");
			printf("> ");		
			// Reset buffer, accept and then evaluate input.	
			memset(buffer, '\0', maxCommand);
			getline(&buffer, &maxCommand, stdin);
			
			// list
			if (strncmp(buffer, "list", strlen(buffer) -1 ) == 0)
			{// implement list command
				printf("You entered list.\n");
			}

			// cache
			else if (strncmp(buffer, "cache", strlen(buffer) - 1) == 0)
			{
				printf("You entered cache.\n");
				// implement cache
			}
			
			// shutdown
			else if (strncmp(buffer, "shutdown", strlen(buffer) -1) == 0)
			{
				//implement shutdown
				printf("You entered shutdown.\n");
			}
			else
			{
				printf("Not a valid command, try again.\n");
			}
		}
		else if (strncmp(buffer, "2", 1) == 0)
		{//Non interactive client commands)

		}	
		else
		{
			printf("Please enter a valid command.\n");
		}
	}	
}


int main(int argc, char **argv)
{
	int listenfd, connfd;
	std::vector<std::thread> threadVect;
	socklen_t   len;
	struct sockaddr_in  servaddr, cliaddr;
	char buff[MAXLINE];
	time_t ticks;

    // Create an end-point for IPv4 Internet Protocol
    if( ( listenfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ) {
	fprintf( stderr, "socket failed.  %s\n", strerror( errno ) );
	exit( 1 );

    }

    if (argc < 2)
    {
	printf("Error, need socket to listen to\n");
	exit(2);
    }

    temp = atoi(argv[1]);	

    

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;             // Communicate using the Internet domain (AF_INET)
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   // Who should we accept connections from?         
    servaddr.sin_port        = htons(PORT_NUMBER);  // Which port should the server listen on?        

    // Bind the server end-point using the specifications stored in "serveraddr"
    if( bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
	fprintf( stderr, "Bind failed.  %s\n", strerror( errno ) );
	exit( 1 );
    }

    // Listen on the in-comming connections; pile up at most LISTENQ number of connections.   
    if( listen(listenfd, LISTENQ) < 0 ) {
	fprintf( stderr, "Listen failed.  %s\n", strerror( errno ) );
	exit( 1 );
    }


	threadVect.push_back(std::thread(userShell, connfd));	




    for ( ; ; ) {
	len = sizeof(cliaddr);
	// establish a connection with an incoming client.
	int newfd;	
	if( (connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len) ) < 0 ) {
	    fprintf( stderr, "Accept failed.  %s\n", strerror( errno ) );
	    exit( 1 );
	}
	else {
		newfd = connfd;
		threadVect.push_back(std::thread(processRequest, newfd));
		printf("connection from %s, port %d\n",
	       	inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
	       	ntohs(cliaddr.sin_port));
		struct Client newClient;
		newClient.IPAddress = buff;
		newClient.port = ntohs(cliaddr.sin_port);
		newClient.connfd = newfd;
		clientConnections.push_back(newClient);
		ticks = time(NULL);
	}
   	
   }
	// finished talking to this client.  Close the connection.
	for (int i = 0; i < threadVect.size(); i++)
	{
		threadVect[i].join();
	}		
	close(connfd);
    
}
