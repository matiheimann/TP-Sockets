#ifndef DATABASE_H
#define DATABASE_H

#define USER_NOT_FOUND 0
#define USER_FOUND 1

#include <sqlite3.h> 

char* handleQuery(char* query);
void addUser(sqlite3* database, int userID);
void deleteUser(sqlite3* database, int userID);
void connectToDatabase(sqlite3* database);
void disconnectFromDatabase(sqlite3* database);
void executeSQLStatement(char* statement, sqlite3* database);
void addSeat(sqlite3* database, int seatNumber, char* state, int flightNumber);
void deleteSeat(sqlite3* database, int seatNumber);
void addFlight(sqlite3* database, int flightNumber);
void deleteFlight(sqlite3* database, int flightNumber);
void addReservation(sqlite3* database, int useriD, int seatNumber, int flightID);
void deleteReservation(sqlite3* database, int userID);
int userExists(sqlite3* database, int userID);
int selectUserCallback(void *data, int argc, char **argv, char **azColName);

#endif