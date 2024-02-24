#ifndef __STDLIB_H__
#define __STDLIB_H__
#include <stdint.h>

const char* itoa(int value, char* str, int base);
const char* ultoa(uint32_t value, char* str, int base);
int atoi(const char* str);

#endif
