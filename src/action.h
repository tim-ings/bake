#ifndef H_ACTION
#define H_ACTION

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "str.h"

#define DEFAULT_SHELL "/bin/bash"

struct Action
{
    char modifier;
    String* command;
} typedef Action;

Action* Action_new(char mod, String* command);
void Action_free(Action* self);
int Action_exec(Action* self);

#endif
