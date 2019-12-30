#include "str.h"


String* String_new(char* str) {
    String* s = malloc(sizeof(String));
    s->str = str;
    s->length = strlen(str);
    return s;
}

int String_len(String* self) {
    return self->length;
}

List* String_split(String* self, const char delim) {
    List* tokens = List_new();
    char* s = strdup(self->str);
    char* tok = strtok(s, &delim);
    while (tok != NULL) {
        List_add(tokens, strdup(tok));
        tok = strtok(NULL, &delim);
    }
    free(tok);
    free(s);
    return tokens;
}
