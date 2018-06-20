#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>  
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h>
#include <pthread.h>
#include "server.h"



int main()
{
	int serverDescriptor;
	int clientDescriptor;
	int * connectionDescriptor
	char message[MESSAGE_SIZE];
	struct sockaddr_un serverAddress, clientAddress;
	socklent_t clientAddressSize;

	
	//New socket
	serverDescriptor = socket(AF_LOCAL, SOCK_STREAM, 0);
	if(socket == -1)
	{
		perror("Socket could not be created");
		exit(1)
	}
	memset(&serverAddress, 0, sizeof(struct sockaddr_un));
	strncpy(serverAddress.sun_path, SOCKET_PATH, sizeof(serverAddress.sun_path));

	//Give name to socket
	if(bind(serverDescriptor, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_un)) == -1)
	{
		perror("Bind error");
		exit(1);
	}

	if(listen(serverDescriptor, LISTEN_BACKLOG) == -1)
	{
		perror("Listen error");
		exit(1);
	}
	while(clientDescriptor = accept(serverDescriptor,(struct sockaddr*)&clientAddress, &clientAddressSize))
	{
		pthread_t thread;
		connectionDescriptor = malloc(sizeof(int));
		*connectionDescriptor = clientDescriptor;
		if(pthread_create(&thread, NULL, connectionSolver, (void*)connectionDescriptor) == -1)
		{
			perror("Thread creation error");
			exit(1);
		}
	}

	if(clientDescriptor = -1)
	{
		perror("Accept error");
		exit(1);
	}
	
}

void connectionSolver(void *connectionDescriptor)
{
	/*Aca se resuelve la conexion con el servidor*/
}