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

bool Variable_eq(void* a, void* b) {
    Variable* var1 = (Variable*)a;
    String* var2_name = (String*)b;
    return String_equals(var1->name, var2_name);
}
