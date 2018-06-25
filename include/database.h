#ifndef DATABASE_H
#define DATABASE_H

#define USER_NOT_FOUND 0
#define USER_FOUND 1
#define FLIGHT_NOT_FOUND 2
#define FLIGHT_FOUND 3
#define AVAILABLE "available"
#define RESERVED "reserved"

#define MAX_AMOUNT_OF_SEATS 20

#include <sqlite3.h>

char *handleQuery(char *query);
void addUser(sqlite3 *database, int userID);
void deleteUser(sqlite3 *database, int userID);
void connectToDatabase(sqlite3 *database);
void disconnectFromDatabase(sqlite3 *database);
void executeSQLStatement(char *statement, sqlite3 *database);
void addSeat(sqlite3 *database, int seatNumber, char *state, int flightNumber);
void deleteSeat(sqlite3 *database, int seatNumber);
void addFlight(sqlite3 *database, int flightNumber);
void deleteFlight(sqlite3 *database, int flightNumber);
void addReservation(sqlite3 *database, int useriD, int seatNumber,
		    int flightID);
void deleteReservation(sqlite3 *database, int userID);
int userExists(sqlite3 *database, int userID);
int selectUserCallback(void *data, int argc, char **argv, char **azColName);
void createANewFlight(sqlite3 *database, int flightNumber);
int flightExists(sqlite3 *database, int flightNumber);
int selectFlightCallback(void *data, int argc, char **argv, char **azColName);
int selectSeatsFromFlight(sqlite3 *database, int flightNumber, char *result);
int selectSeatsFromFlightCallback(void *data, int argc, char **argv,
				  char **colName);
int seatAvailable(int seatNumber, int flight, sqlite3 *database);
int availableSeatCallback(void *data, int argc, char **argv, char **azColName);
void reserveSeat(int seatNumber, int flight, sqlite3 *database);
int seatReserved(int seatNumber, int flight, sqlite3 *database);
int cancelSeatCallback(void *data, int argc, char **argv, char **azColName);
void cancelSeat(int seatNumber, int flight, sqlite3 *database);

#endif