#ifndef H_TARGET
#define H_TARGET

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <curl/curl.h>

#include "action.h"
#include "bool.h"
#include "clargs.h"

struct Target
{
    String* name;
    List* dependecies;
    List* actions;
} typedef Target;

static regex_t re_url;
static bool re_url_compiled = false;

Target* Target_new(String* name, List* deps);
void Target_free(Target* self);
bool Target_eq(void* a, void* b);
void Target_addAction(Target* self, Action* action);
bool Target_isOutDated(Target* self, List* others);
int Target_build(Target* self);

#endif
