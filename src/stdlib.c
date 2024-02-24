#include <stdlib.h>
#include <ctype.h>

const char* itoa(int value, char* str, int base)
{
	char* rc;
	char* ptr;
	char* low;
	
	if (base < 2 || base > 36)
	{
		*str = '\0';
		return str;
	}
	rc = ptr = str;	
	if (value < 0 && base == 10)
	{
		*ptr++ = '-';
	}

	low = ptr;
	do
	{
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
		value /= base;
	} while (value);

	*ptr-- = '\0';
	while (low < ptr)
	{
		char tmp = *low;
		*low++ = *ptr;
		*ptr-- = tmp;
	}
	return rc;
}

const char* ultoa(uint32_t value, char* str, int base)
{
	char* rc;
	char* ptr;
	char* low;
	
	if (base < 2 || base > 36)
	{
		*str = '\0';
		return str;
	}
	rc = ptr = str;	

	low = ptr;
	do
	{
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
		value /= base;
	} while (value);

	*ptr-- = '\0';
	while (low < ptr)
	{
		char tmp = *low;
		*low++ = *ptr;
		*ptr-- = tmp;
	}
	return rc;
}

int atoi(const char* str) {
	int value = 0;
  	while(isdigit(*str)) {
		value *= 10;
		value += (*str)-'0';
		str++;
  	}

  	return value;
}
