#include "include/database.h"
#include <string.h>
#include <sqlite3.h> 
#include <stdio.h>

//Simple test
int main()
{
	addUser("Lorenord", 96);
}

char* handleQuery(char* query)
{
	return "Hi im server!";
}

int addUser(char* username, int userID)
{
   sqlite3 *database;
   char *errorMessage = 0;
   char sqlStatement[60];
   char userIDToString[5]; 
   int connectionReturnValue = sqlite3_open("database.db", &database);

   if(connectionReturnValue != SQLITE_OK)
   {
   		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(database));
   }

   strcpy(sqlStatement,"INSERT INTO user(userID, username) VALUES(");
   sprintf(userIDToString, "%d", userID);
   strcat(sqlStatement, userIDToString);
   strcat(sqlStatement, ",'");
   strcat(sqlStatement, username);
   strcat(sqlStatement, "');");

   connectionReturnValue = sqlite3_exec(database, sqlStatement, NULL, 0, &errorMessage);

   if( connectionReturnValue != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", errorMessage);
      sqlite3_free(errorMessage);
  }

   printf("%s\n", sqlStatement);


  sqlite3_close(database);
}