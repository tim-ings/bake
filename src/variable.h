#ifndef H_VARIABLE
#define H_VARIABLE

#include <stdlib.h>

#include "str.h"


struct Variable
{
    String* name;
    String* value;
} typedef Variable;

Variable* Variable_new(String* name, String* value);
void Variable_free(Variable* self);

#endif
