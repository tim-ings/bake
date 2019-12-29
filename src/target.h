#ifndef H_TARGET
#define H_TARGET

#include "action.h"


struct Target
{
    char* name;
    char** dependecies;
    Action* actions;
    size_t actions_len;
} typedef Target;

#endif