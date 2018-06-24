#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>  
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/un.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>
#include <ctype.h>
#include "server.h"
#include "database.h"
#include "client.h"

int main(int argc, char const *argv[])
{
	int serverDescriptor;
	struct sockaddr_un serverAddress;
	serverDescriptor = socket(AF_UNIX, SOCK_STREAM, 0);
	printf("Starting new client (pid:%d)...\n", getpid());
	if(serverDescriptor == -1)
	{
		perror("Socket could not be created");
		exit(1);
	}
	memset(&serverAddress, 0, sizeof(struct sockaddr_un));
	serverAddress.sun_family = AF_UNIX;
	strncpy(serverAddress.sun_path, SOCKET_PATH, sizeof(serverAddress.sun_path));
	if(connect(serverDescriptor, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) ==-1)
	{
		perror("Bind error");
		exit(1);
	}
	printf("Client (pid:%d) connecting to server...\n", getpid());

	int exit = 0;
	char command[MESSAGE_SIZE];

	int newUserFlag = 0;
	printf("%s\n", "Create new user? (enter y for yes, n for no)\n");
	while(newUserFlag == 0)
	{
		scanf("%s", command);
		if(strcmp(command, "y") == 0)
		{
			printf("%s\n", "Enter your new user id (integer of 3 or less digits): ");
			do 
			{
				memset(command, '\0', MESSAGE_SIZE);
				scanf("%s", command);
			} while(!isNewUserIDValid(command, serverDescriptor)); //If userID is valid, it is created and added to the database
			
			newUserFlag = 1;
			printf("%s\n", "User succesfully created\n");
		}

		if(strcmp(command, "n") == 0)
		{
			printf("%s\n", "Enter your user id: ");
			do 
			{
				memset(command, '\0', MESSAGE_SIZE);
				scanf("%s", command);
			} while(!isUserCreated(command, serverDescriptor));
			

			newUserFlag = 1;
			printf("%s\n", "Login succesful\n");
		}
	}

	printf("%s\n", "What do you want to do?\n");

	while(exit == 0)
	{
		scanf("%s", command);

		if(strcmp(command,"exit") == 0)
		{
			exit = 1;
			write(serverDescriptor, "exit", 4);
		}
	}
	printf("%s\n", "exiting client...\n");
	close(serverDescriptor);
	return 0;
}

int isUserCreated(char* userID, int serverDescriptor)
{
	char* messageToServer = (char*)calloc(MESSAGE_SIZE, sizeof(char));
	char* messageFromServer = (char*)calloc(MESSAGE_SIZE, sizeof(char));

	strcpy(messageToServer, "validate user id: ");
	strcat(messageToServer, userID);

	if(!isUserIDFormatCorrect(userID))
	{
		return 0;
	}

	write(serverDescriptor, messageToServer, strlen(messageToServer)); //Send user id to server for validation
	read(serverDescriptor, messageFromServer, MESSAGE_SIZE); //read result from server

	free(messageToServer);

	if(strcmp(messageFromServer, "user exists") == 0)
	{
		free(messageFromServer);
		return 1;
	}

	printf("%s\n", "User id does not exist, try again: \n");
	free(messageFromServer);
	return 0;
}

int isNewUserIDValid(char* userID, int serverDescriptor)
{
	char* messageToServer = (char*)calloc(MESSAGE_SIZE, sizeof(char));
	char* messageFromServer = (char*)calloc(MESSAGE_SIZE, sizeof(char));

	strcpy(messageToServer, "validate user id: ");
	strcat(messageToServer, userID);

	if(!isUserIDFormatCorrect(userID))
	{
		return 0;
	}

	write(serverDescriptor, messageToServer, strlen(messageToServer)); //Send user id to server for validation
	read(serverDescriptor, messageFromServer, MESSAGE_SIZE); //read result from server

	free(messageToServer);

	if(strcmp(messageFromServer, "user exists") == 0)
	{
		printf("%s\n", "Error, userID already exists, try again:\n");
		free(messageFromServer);
		return 0;
	}

	if(strcmp(messageFromServer, "user does not exist") == 0)
	{
		free(messageFromServer);
		return 1;
	}

	printf("%s\n", "Error while searching user in database, try again:\n");
	return 0;
}


int isUserIDFormatCorrect(char* userID)
{
	int id = atoi(userID);

	//Check if id is a number
	int i;
	for(i=0; i<strlen(userID); i++)
	{
		if(!isdigit(userID[i]))
		{
			printf("%s\n", "Error, userID must be an integer\n");
			return 0;
		}
	}

	//Check amount of digits
	if(((int)(log10(id) + 1)) > 3) //More than three digits, invalid
	{
		printf("%s\n", "Error, userID must be of equal or less than 3 digits, try again:\n");
		return 0;
	}

	return 1;
}