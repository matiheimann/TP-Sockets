#ifndef CLIENT_H
#define CLIENT_H

int isNewUserIDValid(char* userID, int serverDescriptor);
int startsWith(const char *str, const char *start);
int isUserCreated(char* userID, int serverDescriptor);
int isUserIDFormatCorrect(char* userID);
int isValidFlightNumber(char* flightNumber);
int isValidSeatReservation(char* seat, char* flightNumber, int serverDescriptor);
int isValidSeatCancelation(char* seat, char* flightNumber, int serverDescriptor);
int isSeatFormatValid(char* seat);

#endif