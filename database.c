#include "include/database.h"
#include <string.h>
#include <stdio.h>

//Simple test
int main()
{
	sqlite3* database;

	if(sqlite3_open("database.db", &database) != SQLITE_OK)
   	{
   		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(database));
   	}

   	executeSQLStatement("PRAGMA foreign_keys = ON;", database); //Habilito las FOREIGN KEYS en sqlite3

   	addFlight(database, 1);
   	addUser(database, "Lorenord", 96);
   	addSeat(database, 10, "available", 1);
   	addReservation(database, 96, 10, 1);

   	deleteFlight(database, 1); //Al estar ON DELETE CASCADE para las foreign keys, se eliminaran los asientos 
   	  							//y reservas asociados al vuelo eliminado.

	sqlite3_close(database);
}

char* handleQuery(char* query)
{
	return "Hi im server!";
}

void addUser(sqlite3* database, char* username, int userID)
{
   char sqlStatement[100];
   char userIDToString[5]; 

   strcpy(sqlStatement,"INSERT INTO user VALUES(");
   sprintf(userIDToString, "%d", userID);
   strcat(sqlStatement, userIDToString);
   strcat(sqlStatement, ",'");
   strcat(sqlStatement, username);
   strcat(sqlStatement, "');");

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

void executeSQLStatement(char* statement, sqlite3* database)
{
	char *errorMessage = 0;

	if(sqlite3_exec(database, statement, NULL, 0, &errorMessage) != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", errorMessage);
      	sqlite3_free(errorMessage);
	}
}