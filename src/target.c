#include "target.h"


Target* Target_new(char* name, char** deps, size_t deps_len) {
    Target* t = malloc(sizeof(Target));
    t->name = name;
    t->dependecies = deps;
    t->dependecies_len = deps_len;
    t->actions_len = 0;
    t->actions = NULL;
    return t;
}

void Target_free(Target self) {
    for (int i = 0; i < self.actions_len; i++) {
        Action_free(self.actions[i]);
    }
    if (self.actions != NULL) {
        free(self.actions);
    }
    for (int i = 0; i < self.dependecies_len; i++) {
        free(self.dependecies[i]);
    }
    free(self.dependecies);
    free(self.name);
}

void Target_addAction(Target* self, Action* action) {
    self->actions_len++;
    Action* actions = realloc(self->actions, sizeof(Action) * self->actions_len);
    if (!actions) {
        fprintf(stderr, "FATAL: Realloc failed in Target_addAction\n");
        exit(EXIT_FAILURE);
    }
    self->actions = actions; // update the pointer after realloc

    // add our new action
    self->actions[self->actions_len - 1] = *action;
}
