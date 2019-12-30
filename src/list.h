#ifndef H_LIST
#define H_LIST

#include <stdlib.h>
#include <stdio.h>

#define LIST_INIT_CAPACITY 10
#define LIST_CAPACITY_STEP 10


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

#endif
