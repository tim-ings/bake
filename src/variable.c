#include "variable.h"


void Variable_free(Variable self) {
    free(self.name);
    free(self.value);
}