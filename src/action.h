#ifndef H_ACTION
#define H_ACTION

#include "stdlib.h"

struct Action
{
    char modifier;
    char* command;
} typedef Action;

Action* Action_new(char mod, char* command);
void Action_free(Action self);

#endif