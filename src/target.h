#ifndef H_TARGET
#define H_TARGET

#include <stdlib.h>
#include <stdio.h>

#include "action.h"


struct Target
{
    char* name;
    char** dependecies;
    size_t dependecies_len;
    Action* actions;
    size_t actions_len;
} typedef Target;

Target* Target_new(char* name, char** deps, size_t deps_len);
void Target_free(Target self);
void Target_addAction(Target* self, Action* action);

#endif