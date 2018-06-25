#include "include/database.h"
#include <string.h>
#include <stdio.h>

//Simple test
/*int main()
{
	sqlite3* database;

	if(sqlite3_open("database.db", &database) != SQLITE_OK)
   	{
   		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(database));
   	}

   	executeSQLStatement("PRAGMA foreign_keys = ON;", database); //Habilito las FOREIGN KEYS en sqlite3

   	char result[200] = {0};
	selectSeatsFromFlight(database, 2, result);
	printf("%s\n", result);
	sqlite3_close(database);
}*/

char* handleQuery(char* query)
{
	return "Hi im server!";
}

void addUser(sqlite3* database, int userID)
{
   char sqlStatement[50];
   char userIDToString[4]; 

   strcpy(sqlStatement,"INSERT INTO user VALUES(");
   sprintf(userIDToString, "%d", userID);
   strcat(sqlStatement, userIDToString);
   strcat(sqlStatement, ");");

   executeSQLStatement(sqlStatement, database);
}

void deleteUser(sqlite3* database, int userID)
{
	char sqlStatement[60];
	char userIDToString[5]; 

	strcpy(sqlStatement,"DELETE FROM user WHERE userID=");
	sprintf(userIDToString, "%d", userID);
	strcat(sqlStatement, userIDToString);
	strcat(sqlStatement, ";");

	executeSQLStatement(sqlStatement, database);
}

void addSeat(sqlite3* database, int seatNumber, char* state, int flightNumber)
{
	char sqlStatement[100];
	char seatNumberToString[2]; 
	char flightNumberToString[3];

	strcpy(sqlStatement,"INSERT INTO seat VALUES(");
	sprintf(seatNumberToString, "%d", seatNumber);
	strcat(sqlStatement, seatNumberToString);
	strcat(sqlStatement, ",'");
	strcat(sqlStatement, state);
	strcat(sqlStatement, "',");
	sprintf(flightNumberToString, "%d", flightNumber);
	strcat(sqlStatement, flightNumberToString);
	strcat(sqlStatement, ");");

	executeSQLStatement(sqlStatement, database);
}

void deleteSeat(sqlite3* database, int seatNumber)
{
	char sqlStatement[100];
	char seatNumberToString[2]; 

	strcpy(sqlStatement,"DELETE FROM seat WHERE seatNumber=");
	sprintf(seatNumberToString, "%d", seatNumber);
	strcat(sqlStatement, seatNumberToString);
	strcat(sqlStatement, ";");

	executeSQLStatement(sqlStatement, database);
}

void addFlight(sqlite3* database, int flightNumber)
{
	char sqlStatement[50];
	char flightNumberToString[3];

	strcpy(sqlStatement,"INSERT INTO flight VALUES(");
	sprintf(flightNumberToString, "%d", flightNumber);
	strcat(sqlStatement, flightNumberToString);
	strcat(sqlStatement, ");");

	executeSQLStatement(sqlStatement, database);
}

void deleteFlight(sqlite3* database, int flightNumber)
{
	char sqlStatement[50];
	char flightNumberToString[3];

	strcpy(sqlStatement,"DELETE FROM flight WHERE flightID=");
	sprintf(flightNumberToString, "%d", flightNumber);
	strcat(sqlStatement, flightNumberToString);
	strcat(sqlStatement, ";");

	executeSQLStatement(sqlStatement, database);
}

void addReservation(sqlite3* database, int userID, int seatNumber, int flightNumber)
{
	char sqlStatement[50];
	char flightNumberToString[3];
	char seatNumberToString[2];
	char userIDToString[5];

	strcpy(sqlStatement,"INSERT INTO reservation VALUES(");
	sprintf(userIDToString, "%d", userID);
	strcat(sqlStatement, userIDToString);
	strcat(sqlStatement, ",");
	sprintf(seatNumberToString, "%d", seatNumber);
	strcat(sqlStatement, seatNumberToString);
	strcat(sqlStatement, ",");
	sprintf(flightNumberToString, "%d", flightNumber);
	strcat(sqlStatement, flightNumberToString);
	strcat(sqlStatement, ");");

	executeSQLStatement(sqlStatement, database);
}

void deleteReservation(sqlite3* database, int userID)
{
	char sqlStatement[50];
	char userIDToString[5];

	strcpy(sqlStatement,"DELETE FROM reservation WHERE userID=");
	sprintf(userIDToString, "%d", userID);
	strcat(sqlStatement, userIDToString);
	strcat(sqlStatement, ";");

	executeSQLStatement(sqlStatement, database);
}

int userExists(sqlite3* database, int userID)
{
	char sqlStatement[50];
	char userIDToString[5];

	strcpy(sqlStatement, "SELECT * FROM user WHERE userID=");
	sprintf(userIDToString, "%d", userID);
	strcat(sqlStatement, userIDToString);
	strcat(sqlStatement, ";");

	char *errorMessage = 0;
	int userFound = USER_NOT_FOUND;

	if(sqlite3_exec(database, sqlStatement, selectUserCallback, (int*)&userFound, &errorMessage) != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", errorMessage);
      	sqlite3_free(errorMessage);
	}	

	return userFound == USER_FOUND ? 1 : 0;
}

int selectUserCallback(void *data, int argc, char **argv, char **azColName)
{
   	*((int*)data) = USER_FOUND;
   	return 0;
}

int flightExists(sqlite3* database, int flightNumber)
{
	char sqlStatement[50];
	char flightNumberToString[5];

	strcpy(sqlStatement, "SELECT * FROM flight WHERE flightID=");
	sprintf(flightNumberToString, "%d", flightNumber);
	strcat(sqlStatement, flightNumberToString);
	strcat(sqlStatement, ";");

	char *errorMessage = 0;
	int flightFound = FLIGHT_NOT_FOUND;

	if(sqlite3_exec(database, sqlStatement, selectFlightCallback, (int*)&flightFound, &errorMessage) != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", errorMessage);
      	sqlite3_free(errorMessage);
	}	

	return flightFound == FLIGHT_FOUND ? 1 : 0;
}

int selectFlightCallback(void *data, int argc, char **argv, char **azColName)
{
   	*((int*)data) = FLIGHT_FOUND;
   	return 0;
}

void createANewFlight(sqlite3* database, int flightNumber)
{
	addFlight(database, flightNumber);
	int i;
	for(i=1; i<=MAX_AMOUNT_OF_SEATS; i++)
	{
		addSeat(database, i, AVAILABLE, flightNumber);
	}
}

int selectSeatsFromFlight(sqlite3* database, int flightNumber, char* result)
{
	char sqlStatement[50];
	char flightNumberToString[5];

	strcpy(sqlStatement, "SELECT * FROM seat WHERE flightNumber=");
	sprintf(flightNumberToString, "%d", flightNumber);
	strcat(sqlStatement, flightNumberToString);
	strcat(sqlStatement, ";");

	char *errorMessage = 0;

	if(sqlite3_exec(database, sqlStatement, selectSeatsFromFlightCallback, result, &errorMessage) != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", errorMessage);
      	sqlite3_free(errorMessage);
	}	

	if(strlen(result)==0)
	{
		return 0;
	}

	return 1;
}

int selectSeatsFromFlightCallback(void *data, int argc, char **argv, char **colName)
{   
   int i;   
   char* result = (char*)data;
   for(i = 0; i<argc; i++)
   {
   	  if(strcmp(colName[i], "flightNumber") != 0)
   	  {
   	  	if(strcmp(colName[i], "seatState") == 0)
   	  	{
   	  		if(strcmp(argv[i], "available") == 0)
   	  		{
   	  			strcat(result, "A) ");
   	  		}
   	  		else if(strcmp(argv[i], "reserved") == 0)
			{
				strcat(result, "R) ");
			}   	  		
   	  	}
   	  	if(strcmp(colName[i], "seatNumber") == 0)
   	  	{
   	  		strcat(result, argv[i]);
   	  		strcat(result, "(");
   	  	}
   	  }
   }
   
   return 0;
}

void executeSQLStatement(char* statement, sqlite3* database)
{
	char *errorMessage = 0;

	if(sqlite3_exec(database, statement, NULL, 0, &errorMessage) != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", errorMessage);
      	sqlite3_free(errorMessage);
	}
}