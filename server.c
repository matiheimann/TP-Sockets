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
#include "server.h"
#include "database.h"
#include "utilities.h"

int main()
{
	int serverDescriptor;
	int clientDescriptor;
	int * connectionDescriptor;
	struct sockaddr_un serverAddress, clientAddress;
	socklen_t clientAddressSize = sizeof(struct sockaddr_un);
	printf("Starting new server (pid:%d)...\n", getpid());
	
	//New socket
	serverDescriptor = socket(AF_UNIX, SOCK_STREAM, 0);
	if(serverDescriptor == -1)
	{
		perror("Socket could not be created");
		exit(1);
	}

	memset(&serverAddress, 0, sizeof(struct sockaddr_un));
	serverAddress.sun_family = AF_UNIX;
	strncpy(serverAddress.sun_path, SOCKET_PATH, sizeof(serverAddress.sun_path));
	unlink(SOCKET_PATH);
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
	printf("Server accepting connections...\n");
	while((clientDescriptor = accept(serverDescriptor,(struct sockaddr*)&clientAddress, &clientAddressSize)))
	{
		//Create thread 
		pthread_t thread;
		connectionDescriptor = malloc(sizeof(int));
		*connectionDescriptor = clientDescriptor;
		if(pthread_create(&thread, NULL, (void * (*)(void *))connectionSolver, (void*)connectionDescriptor) == -1)
		{
			perror("Thread creation error");
			exit(1);
		}
	}

	if(clientDescriptor == -1)
	{
		perror("Accept error");
		exit(1);
	}
	return 0;
	
}

//This functions handles the connection with the client
void* connectionSolver(void *connectionDescriptor)
{
	int descriptor = *((int *)connectionDescriptor);
	char* buffer = calloc(MESSAGE_SIZE, sizeof(char));
	char* userIDString;
	char* seat;
	char* loggedInUserID;
	char* flightNumber;
	int userID;
	int seatNumber;
	int flight;

	sqlite3* database;

	if(sqlite3_open("database.db", &database) != SQLITE_OK) //Connect to database
   	{
   		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(database));
   	}

   	executeSQLStatement("PRAGMA foreign_keys = ON;", database); //Enable foreign keys in sqlite3

   	char* message;
	int exit = 0;
	while(exit == 0)
	{
		memset(buffer, '\0', MESSAGE_SIZE); //Clear buffer before reading

		read(descriptor, buffer, MESSAGE_SIZE);

		if(startsWith(buffer, "AF"))	//Add flight number
		{
			flightNumber = buffer + 2;
			printf("%s%s\n", "Adding flight number received: ", flightNumber);
			if(flightExists(database, atoi(flightNumber)))
			{
				message = "Flight already exists\n";
				write(descriptor, message, strlen(message));
			}
			else
			{
				createANewFlight(database, atoi(flightNumber));
				message = "Flight succesfully created\n";
				write(descriptor, message, strlen(message));
			}
		}
		if(startsWith(buffer, "DF"))	//Delete flight number
		{
			flightNumber = buffer + 2;
			printf("%s%s\n", "Deleting flight number received: ", flightNumber);
			if(flightExists(database, atoi(flightNumber)))
			{
				deleteFlight(database, atoi(flightNumber));
				message = "Flight succesfully deleted\n";
				write(descriptor, message, strlen(message));
			}
			else
			{
				message = "Flight does not exist\n";
				write(descriptor, message, strlen(message));
			}
		}
		if(startsWith(buffer, "F"))
		{
			char* result = calloc(MESSAGE_SIZE, sizeof(char));

			flightNumber = buffer + 1;
			printf("%s%s\n", "Checking flight number state: ", flightNumber);

			if(flightExists(database, atoi(flightNumber)))
			{
				selectSeatsFromFlight(database, atoi(flightNumber), result);
				strcat(result, "\n");
				printf("%s\n%s\n", "Flight exists, disposition: ", result);
				write(descriptor, result, strlen(result));
			}
			else
			{
				message = "Flight does not exist\n";
				write(descriptor, message, strlen(message));
			}
			free(result);

		}

		if(startsWith(buffer, "validate user id: "))
		{
			userIDString = buffer + 18;
			userID = atoi(userIDString);
			printf("Validating user id %d\n", userID);

			if(userExists(database, userID))
			{
				write(descriptor, "user exists", 11);
			}
			else
			{
				write(descriptor, "user does not exist", 19);
				addUser(database, userID); //Create new user in database
				loggedInUserID = userIDString;
				printf("%s%s\n", loggedInUserID, " logged in\n");
			}
		}

		if(startsWith(buffer, "validate seat reservation: "))
		{
			printf("Buffer: %s\n", buffer);
			seat = buffer + 27;
			seatNumber = atoi(seat);
			flightNumber = buffer + 29;
			flight = atoi(flightNumber);
			printf("Validating seat reservation, seat:%d, flight:%d\n", seatNumber, flight);

			if(seatAvailable(seatNumber, flight, database))
			{
				write(descriptor, "available", 9);
				reserveSeat(seatNumber, flight, database);
			}
			else
			{
				write(descriptor, "reserved", 8);
			}
		}

		if(startsWith(buffer, "validate existing user id: "))
		{
			userIDString = buffer + 27;
			userID = atoi(userIDString);
			printf("Validating user id %d\n", userID);

			if(userExists(database, userID))
			{
				write(descriptor, "user exists", 11);
				loggedInUserID = userIDString;
				printf("%s%s\n", loggedInUserID, " logged in\n");
			}
			else
			{
				write(descriptor, "user does not exist", 19);
			}
		}

		if(strcmp(buffer, "exit") == 0)
		{
			exit = 1;
			printf("%s\n", "exit\n");
		}
	}

	sqlite3_close(database);

	free(buffer);
	free(connectionDescriptor);
	//char* answer = handleQuery(buffer);
	//write(descriptor,answer, strlen(answer));
	return NULL;
}