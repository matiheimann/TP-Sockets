#include <ctype.h>
#include <math.h>
#include <string.h>
#include <utilities.h>

int startsWith(const char *str, const char *start)
{
	while (*str && *start) {
		if (*str != *start)
			return 0;
		str++;
		start++;
	}

	return 1;
}

int isNumber(char *string)
{
	int i;
	for (i = 0; i < strlen(string); i++) {
		if (!isdigit(string[i])) {
			return 0;
		}
	}

	return 1;
}

int hasMoreThanThreeDigits(int number)
{
	return (((int)(log10(number) + 1)) > 3);
}