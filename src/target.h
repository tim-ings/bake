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
    String* name;
    List* dependecies;
    List* actions;
} typedef Target;

Target* Target_new(String* name, List* deps);
void Target_free(Target* self);
void Target_addAction(Target* self, Action* action);
// bool Target_isOutDated(Target* self, Target* others, int others_len);
// int Target_build(Target* self);

#endif
