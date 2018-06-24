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