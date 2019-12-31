#include "target.h"


Target* Target_new(String* name, List* deps) {
    Target* t = malloc(sizeof(Target));
    t->name = name;
    t->dependecies = deps;
    t->actions = List_new();
    return t;
}

void Target_free(Target* self) {
    for (int i = 0; i < self->actions->length; i++) {
        Action_free(List_get(self->actions, i));
    }
    List_free(self->actions);
    for (int i = 0; i < self->dependecies->length; i++) {
        String_free(List_get(self->dependecies, i));
    }
    List_free(self->dependecies);
    String_free(self->name);
    free(self);
}

bool Target_eq(void* a, void* b) {
    Target* tar1 = (Target*)a;
    String* tar2_name = (String*)b;
    return String_equals(tar1->name, tar2_name);
}

void Target_addAction(Target* self, Action* action) {
    List_add(self->actions, action);
}

bool Target_isOutDated(Target* self, List* others) {
    // If there are no dependencies, then the target requires rebuilding
    if (self->dependecies->length == 0) {
        printf("Target \"%s\" has no dependencies so requires rebuilding\n", self->name->str);
        return true;
    }

    // If any dependency does not exist, or has been modified more recently 
    // than its target, then the target requires rebuilding

    // check every dependency
    for (int di = 0; di < self->dependecies->length; di++) {
        String* dep = List_get(self->dependecies, di);
        
        // check if the dependency is another target
        Target* other = List_find(others, Target_eq, dep);
        if (other != NULL && Target_isOutDated(other, others)) {
            printf("Target \"%s\" is dependent on an outdated target \"%s\"\n", self->name->str, other->name->str);
            return true;
        }

        // check if the target file exists
        printf("Checking if \"%s\" exists\n", self->name->str);
        if (access(self->name->str, F_OK) == -1) {
            printf("Target \"%s\" does not exist, we need to rebuild \"%s\"\n", self->name->str, self->name->str);
            return true;
        }

        // check if the dependant file exists
        printf("Checking if \"%s\" exists\n", dep->str);
        if (access(dep->str, F_OK) == -1) {
            printf("Dep \"%s\" does not exist, we need to rebuild \"%s\"\n", dep->str, self->name->str);
            return true;
        }

        // check if the dependant file has been modified more recently than the target file
        struct stat depstats;
        struct stat tarstats;
        stat(dep->str, &depstats);
        stat(self->name->str, &tarstats);
        printf("Target \"%s\" was last modifed at: %ld\n", self->name->str, tarstats.st_mtime);
        printf("Dependency \"%s\" was last modifed at: %ld\n", dep->str, depstats.st_mtime);
        if (depstats.st_mtime > tarstats.st_mtime) {
            printf("Dependency \"%s\" has been modififed, we need to rebuild \"%s\"\n", dep->str, self->name->str);
            return true;
        }

        printf("Dependency \"%s\" is not out dated!\n", dep->str);
    }


    return false;
}

int Target_build(Target* self) {
    printf("\tBuilding \"%s\"\n", self->name->str);
    // run all actions for this target
    for (int ai = 0; ai < self->actions->length; ai++) {
        Action* act = List_get(self->actions, ai);
        int res = Action_exec(act);
        return res;
    }
    
    return 0;
}
