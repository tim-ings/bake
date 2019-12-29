#ifndef H_ACTION
#define H_ACTION

#include "stdlib.h"

struct Action
{
    char modifier;
    char* command;
} typedef Action;

void Action_free(Action self);

#endif