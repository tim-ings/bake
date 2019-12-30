#ifndef H_STR
#define H_STR

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#include "list.h"
#include "bool.h"


struct String {
    char* str;
    int length;
} typedef String;

struct ReMatch {
    String* match;
    int start;
    int end;
} typedef ReMatch;

String* String_new(char* str);
void String_free(String* self);
String* String_from_int(int i);
String* String_from_float(float f);
int String_len(String* self);
List* String_split(String* self, char delim);
bool String_equals(String* self, String* other);
bool String_equals_c(String* self, const char* other);
List* String_match(String* self, regex_t* pattern, int matchCount);
String* String_slice(String* self, int start, int end);
String* String_concat(String* self, String* other);

#endif
