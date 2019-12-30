#include "action.h"


Action* Action_new(char mod, char* command) {
    Action* action = malloc(sizeof(Action));
    action->modifier = mod;
    action->command = command;
    return action;
}

void Action_free(Action self) {
    free(self.command);
}