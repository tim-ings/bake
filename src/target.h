#ifndef H_TARGET
#define H_TARGET

#include "action.h"
#include "stdlib.h"


struct Target
{
    char* name;
    char** dependecies;
    size_t dependecies_len;
    Action* actions;
    size_t actions_len;
} typedef Target;

void Target_free(Target self);

#endif