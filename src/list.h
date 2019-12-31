#ifndef H_LIST
#define H_LIST

#include <stdlib.h>
#include <stdio.h>

#include "bool.h"

#define LIST_INIT_CAPACITY 10
#define LIST_CAPACITY_STEP 10

typedef bool (*eq)(void*, void*);

struct List {
    void** items;
    int length;
    int capacity;
} typedef List;

List* List_new();
void List_free(List* self);
void* List_get(List* self, int i);
void List_set(List* self, int i, void* obj);
void List_add(List* self, void* obj);
void* List_find(List* self, eq eq, void* other);

#endif
