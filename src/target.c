#include "target.h"


void Target_free(Target self) {
    for (int i = 0; i < self.actions_len; i++) {
        Action_free(self.actions[i]);
    }
    free(self.actions);
    for (int i = 0; i < self.dependecies_len; i++) {
        free(self.dependecies[i]);
    }
    free(self.dependecies);
    free(self.name);
}
