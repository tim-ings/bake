#include "variable.h"


Variable* Variable_new(String* name, String* value){
    Variable* var = malloc(sizeof(Variable));
    var->name = name;
    var->value = value;
    return var;
}

void Variable_free(Variable* self) {
    String_free(self->name);
    String_free(self->value);
    free(self);
}
