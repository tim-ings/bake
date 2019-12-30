#ifndef H_ACTION
#define H_ACTION

#include <stdlib.h>

#include "str.h"


struct Action
{
    char modifier;
    String* command;
} typedef Action;

Action* Action_new(char mod, String* command);
void Action_free(Action* self);

#endif
