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
#include "utilities.h"

int main(int argc, char const *argv[])
{
	int serverDescriptor;
	struct sockaddr_un serverAddress;
	serverDescriptor = socket(AF_UNIX, SOCK_STREAM, 0);
	char* loggedInUserID;

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
			loggedInUserID = command;
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
			loggedInUserID = command;
		}
	}

	char* flightNumber;
	char* messageFromServer = (char*)calloc(MESSAGE_SIZE, sizeof(char));

	printf("%s%s\n\n", "Logged in with user id: ", loggedInUserID);
	printf("%s\n", "Available commands: \n");
	printf("%s\n", "* Add flight (enter 'AFn', where n is the flight number. Flight numbers cant have more than 3 digits)\n");
	printf("%s\n", "* Delete flight (enter 'DFn', where n is the flight number. Flight numbers cant have more than 3 digits)\n");
	printf("%s\n", "* Check flight state (enter 'Fn' where n is the flight number)\n  In the list presented after executing this command, A stands for available and R stands for reserved\n");
	printf("%s\n", "* Reserve a seat in a given flight (enter 'Rn' where n is the flight number to see available seats\n");
	while(exit == 0)
	{
		scanf("%s", command);

		if(startsWith(command, "R"))
		{
			char* aux = (char*)calloc(MESSAGE_SIZE, sizeof(char));
			char* selectedFlight = (char*)calloc(MESSAGE_SIZE, sizeof(char));
			strcpy(selectedFlight, command+1);
			if(isValidFlightNumber(selectedFlight))
			{	
				strcat(aux,"F");
				strcat(aux,selectedFlight);
				write(serverDescriptor, aux, strlen(aux)); //Write to server Fn command to see seat disposition
				memset(messageFromServer, '\0', MESSAGE_SIZE);
				read(serverDescriptor, messageFromServer, MESSAGE_SIZE);
				printf("%s\n", messageFromServer);
				printf("Enter which seat you would like to reserve: ");
				do
				{
					memset(command, '\0', MESSAGE_SIZE);
					scanf("%s", command);
				}while(!isValidSeatReservation(command, selectedFlight, serverDescriptor));
			}
			write(serverDescriptor, aux, strlen(aux));
			memset(messageFromServer, '\0', MESSAGE_SIZE);
			read(serverDescriptor, messageFromServer, MESSAGE_SIZE);
			printf("%s\n", messageFromServer);
			free(aux);
			free(selectedFlight);
		}

		if(startsWith(command, "AF"))
		{
			flightNumber = command + 2;
			if(isValidFlightNumber(flightNumber))
			{
				write(serverDescriptor, command, strlen(command));
				memset(messageFromServer, '\0', MESSAGE_SIZE);
				read(serverDescriptor, messageFromServer, MESSAGE_SIZE);
				printf("%s\n", messageFromServer);
			}
		}
		if(startsWith(command, "DF"))
		{
			flightNumber = command + 2;
			if(isValidFlightNumber(flightNumber))
			{
				write(serverDescriptor, command, strlen(command));
				memset(messageFromServer, '\0', MESSAGE_SIZE);
				read(serverDescriptor, messageFromServer, MESSAGE_SIZE);
				printf("%s\n", messageFromServer);
			}
		}
		if(startsWith(command, "F"))
		{
			flightNumber = command + 1;
			if(isValidFlightNumber(flightNumber))
			{
				write(serverDescriptor, command, strlen(command));
				memset(messageFromServer, '\0', MESSAGE_SIZE);
				read(serverDescriptor, messageFromServer, MESSAGE_SIZE);
				printf("%s\n", messageFromServer);
			}
		}
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

	strcpy(messageToServer, "validate existing user id: ");
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

	printf("%s\n", "User id does not exist, try again: ");
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
		printf("%s\n", "Error, userID already exists, try again:");
		free(messageFromServer);
		return 0;
	}

	if(strcmp(messageFromServer, "user does not exist") == 0)
	{
		free(messageFromServer);
		return 1;
	}

	printf("%s\n", "Error while searching user in database, try again:");
	return 0;
}


int isUserIDFormatCorrect(char* userID)
{
	int id = atoi(userID);

	if(!isNumber(userID))
	{
		printf("%s\n", "Error, userID must be an integer");
		return 0;
	}

	if(hasMoreThanThreeDigits(id)) 
	{
		printf("%s\n", "Error, userID must be of equal or less than 3 digits, try again:");
		return 0;
	}

	return 1;
}

int isValidFlightNumber(char* flightNumber)
{

	if(!isNumber(flightNumber))
	{
		printf("%s\n", "Error, flight number must be an integer");
		return 0;
	}

	if(hasMoreThanThreeDigits(atoi(flightNumber))) 
	{
		printf("%s\n", "Error, flight number must be of equal or less than 3 digits, try again:");
		return 0;
	}

	return 1;
}

int isValidSeatReservation(char* seat, char* flightNumber, int serverDescriptor)
{
	char* messageToServer = (char*)calloc(MESSAGE_SIZE, sizeof(char));
	char* messageFromServer = (char*)calloc(MESSAGE_SIZE, sizeof(char));

	strcpy(messageToServer, "validate seat reservation: ");
	strcat(messageToServer, seat);
	strcat(messageToServer, " ");
	strcat(messageToServer, flightNumber);

	if(!isSeatFormatValid(seat))
	{
		return 0;
	}

	write(serverDescriptor, messageToServer, strlen(messageToServer)); //Send seat number to server for validation
	read(serverDescriptor, messageFromServer, MESSAGE_SIZE); //read result from server

	free(messageToServer);

	if(strcmp(messageFromServer, "available") == 0)
	{
		free(messageFromServer);
		return 1;
	}

	if(strcmp(messageFromServer, "reserved") == 0)
	{
		printf("%s\n", "Seat is already reserved, choose another\n");
		free(messageFromServer);
		return 0;
	}

	printf("%s\n", "Error while searching seat in database, try again:");
	return 0;	
}

int isSeatFormatValid(char* seat)
{
	int seatNumber = atoi(seat);

	if(!isNumber(seat))
	{
		printf("%s\n", "Error, the seat must be a number, try again\n");
		return 0;
	}

	if(seatNumber<0 || seatNumber>20)
	{
		printf("%s\n", "Error, seats numbers are between 1 and 20, try again\n");
	}

	return 1;
}
