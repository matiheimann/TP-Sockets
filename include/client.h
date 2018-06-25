#ifndef CLIENT_H
#define CLIENT_H

int isNewUserIDValid(char* userID, int serverDescriptor);
int startsWith(const char *str, const char *start);
int isUserCreated(char* userID, int serverDescriptor);
int isUserIDFormatCorrect(char* userID);
int isValidFlightNumber(char* flightNumber);

#endif