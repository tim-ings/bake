#include "list.h"


List* List_new() {
    List* l = malloc(sizeof(List));
    l->length = 0;
    l->capacity = LIST_INIT_CAPACITY;
    l->items = malloc(sizeof(void*) * l->capacity);
    return l;
}

void List_free(List* self) {
    free(self->items);    
    free(self);
}

void* List_get(List* self, int i) {
    return self->items[i];
}

void List_set(List* self, int i, void* obj) {
    self->items[i] = obj;
}

void List_add(List* self, void* obj) {
    if (self->capacity <= self->length) {
        self->capacity += LIST_CAPACITY_STEP;
        void** items = realloc(self->items, sizeof(void*) * self->capacity);
        if (items == NULL) {
            fprintf(stderr, "FATAL: Failed to call realloc in List_add\n");
            exit(EXIT_FAILURE);
        }
        self->items = items;
    }
    self->items[self->length++] = obj;
}

void* List_find(List* self, eq eq, void* other) {
    for (int i = 0; i < self->length; i++) {
        void* si = List_get(self, i);
        if (eq(si, other)) {
            return si;
        }
    }
    return NULL;
}
