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

bool Target_isOutDated(Target* self, Target* others, int others_len) {
    // If there are no dependencies, then the target requires rebuilding
    if (self->dependecies_len == 0) return true;

    // If any dependency does not exist, or has been modified more recently 
    // than its target, then the target requires rebuilding

    // check every dependency
    for (int di = 0; di < self->dependecies_len; di++) {
        char* dep = self->dependecies[di];
        
        // check if the dependency is another target
        for (int ti = 0; ti < others_len; ti++) {
            Target* othertar = &others[ti];
            if (strcmp(dep, othertar->name) == 0) {
                // if the dependency is outdated this target is outdated
                if (Target_isOutDated(othertar, others, others_len)) return true;
            }
        }

        // check if the dependency is a file
        struct stat depstats;
        struct stat tarstats;
        stat(dep, &depstats);
        stat(self->name, &tarstats);
        printf("File (tar) \"%s\" was last modifed at: %ld\n", self->name, tarstats.st_mtime);
        printf("File (dep) \"%s\" was last modifed at: %ld\n", dep, depstats.st_mtime);
        if (depstats.st_mtime > tarstats.st_mtime) {
            printf("Dep \"%s\" has been modififed, we need to rebuild \"%s\"\n", dep, self->name);
            return true;
        }
        printf("Checking if \"%s\" exists\n", dep);
        if (access(dep, F_OK) == -1) {
            printf("Dep \"%s\" does not exist, we need to rebuild \"%s\"\n", dep, self->name);
            return true;
        }
        printf("Checking if \"%s\" exists\n", self->name);
        if (access(self->name, F_OK) == -1) {
            printf("Target \"%s\" does not exist, we need to rebuild \"%s\"\n", self->name, self->name);
            return true;
        }

        printf("Dep \"%s\" is not out dated!\n", dep);
    }


    return false;
}

int Target_build(Target* self) {
    printf("Building \"%s\"", self->name);
    return 0;
}