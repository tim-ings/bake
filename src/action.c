#include "action.h"


void Action_free(Action self) {
    free(self.command);
}