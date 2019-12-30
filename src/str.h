#ifndef H_STR
#define H_STR

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"


struct String {
    char* str;
    int length;
} typedef String;

String* String_new(char* str);
int String_len(String* self);
List* String_split(String* self, char delim);

#endif