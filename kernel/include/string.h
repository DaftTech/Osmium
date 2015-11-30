#ifndef STRING_H
#define STRING_H

#include "stddef.h"
#include "stdmem.h"

unsigned int strlen(const char* str);
int          strcmp(const char* str1, const char* str2);
char*        strtok(char* str, const char* delimiters);
char*        strtoknc(char* str, const char* delimiters);
char*        strcpy(char* dest, const char* src);
char*        strclone(const char* src);

#endif
