#ifndef H_TARGET
#define H_TARGET

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "action.h"
#include "bool.h"

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
bool Target_isOutDated(Target* self, Target* others, int others_len);
int Target_build(Target* self);

#endif