#ifndef H_VARIABLE
#define H_VARIABLE

#include <stdlib.h>


struct Variable
{
    char* name;
    char* value;
} typedef Variable;

void Variable_free(Variable self);

#endif