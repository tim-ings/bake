#include "str.h"


void ReMatch_free(ReMatch* self) {
    String_free(self->match);
    free(self);
}

String* String_new(char* str) {
    String* s = malloc(sizeof(String));
    s->str = str;
    s->length = strlen(str);
    return s;
}

void String_free(String* self){
    if (self->str != NULL)
        free(self->str);
    free(self);
}

bool String_eq(void* a, void* b) {
    return String_equals((String*)a, (String*)b);
}

String* String_copy(String* self) {
    return String_new(strdup(self->str));
}

String* String_from_int(int i) {
    char* s = malloc(sizeof(char) * 15);
    sprintf(s, "%d", i);
    return String_new(s);
}

String* String_from_float(float i) {
    char* s = malloc(sizeof(char) * 30);
    sprintf(s, "%f", i);
    return String_new(s);
}

int String_len(String* self) {
    return self->length;
}

List* String_split(String* self, const char delim) {
    List* tokens = List_new();
    char* s = strdup(self->str);
    char* tok = strtok(s, &delim);
    while (tok != NULL) {
        List_add(tokens, String_new(strdup(tok)));
        tok = strtok(NULL, &delim);
    }
    free(tok);
    free(s);
    return tokens;
}

bool String_equals(String* self, String* other) {
    return strcmp(self->str, other->str) == 0;
}

bool String_equals_c(String* self, const char* other) {
    return strcmp(self->str, other) == 0;
}

List* String_match(String* self, regex_t* pattern, int match_count) {
    regmatch_t midx[match_count];
    if (regexec(pattern, self->str, match_count, midx, 0) != REG_NOMATCH) {
        List* matches = List_new();
        for (int i = 0; i < match_count; i++) {
            int match_len = midx[i].rm_eo - midx[i].rm_so;
            char* match = malloc(sizeof(char) * (match_len + 1));
            strncpy(match, &self->str[midx[i].rm_so], match_len);
            match[match_len] = '\0';
            ReMatch* rematch = malloc(sizeof(ReMatch));
            rematch->match = String_new(match);
            rematch->start = midx[i].rm_so;
            rematch->end = midx[i].rm_eo;
            List_add(matches, rematch);
        }
        return matches;
    }
    return NULL;
}

String* String_slice(String* self, int start, int end) {
    int len = end - start;
    char* str = malloc(sizeof(char) * (len + 1));
    strncpy(str, &self->str[start], len);
    str[len] = '\0';
    return String_new(str);
}

String* String_concat(String* self, String* other) {
    int len = self->length + other->length;
    char* str = malloc(sizeof(char) * (len + 1));
    strncpy(str, self->str, self->length);
    strncpy(&str[self->length], other->str, other->length);
    str[len] = '\0';
    return String_new(str);
}
