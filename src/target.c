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

void Target_addAction(Target* self, Action* action) {
    List_add(self->actions, action);
}

// TODO: move to bakefile this is ugly
// bool Target_isOutDated(Target* self, List* others, int others_len) {
//     // If there are no dependencies, then the target requires rebuilding
//     if (self->dependecies->length == 0) return true;

//     // If any dependency does not exist, or has been modified more recently 
//     // than its target, then the target requires rebuilding

//     // check every dependency
//     for (int di = 0; di < self->dependecies->length; di++) {
//         String* dep = List_get(self->dependecies, di);
        
//         // check if the dependency is another target
//         for (int ti = 0; ti < others_len; ti++) {
//             Target* othertar = &others[ti];
//             if (strcmp(dep, othertar->name) == 0) {
//                 // if the dependency is outdated this target is outdated
//                 if (Target_isOutDated(othertar, others, others_len)) return true;
//             }
//         }

//         // check if the dependency is a file
//         struct stat depstats;
//         struct stat tarstats;
//         stat(dep, &depstats);
//         stat(self->name, &tarstats);
//         printf("File (tar) \"%s\" was last modifed at: %ld\n", self->name, tarstats.st_mtime);
//         printf("File (dep) \"%s\" was last modifed at: %ld\n", dep, depstats.st_mtime);
//         if (depstats.st_mtime > tarstats.st_mtime) {
//             printf("Dep \"%s\" has been modififed, we need to rebuild \"%s\"\n", dep, self->name);
//             return true;
//         }
//         printf("Checking if \"%s\" exists\n", dep);
//         if (access(dep, F_OK) == -1) {
//             printf("Dep \"%s\" does not exist, we need to rebuild \"%s\"\n", dep, self->name);
//             return true;
//         }
//         printf("Checking if \"%s\" exists\n", self->name);
//         if (access(self->name, F_OK) == -1) {
//             printf("Target \"%s\" does not exist, we need to rebuild \"%s\"\n", self->name, self->name);
//             return true;
//         }

//         printf("Dep \"%s\" is not out dated!\n", dep);
//     }


//     return false;
// }

// int Target_build(Target* self) {
//     printf("Building \"%s\"", self->name);
//     return 0;
// }
