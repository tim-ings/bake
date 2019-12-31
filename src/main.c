#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "action.h"
#include "variable.h"
#include "bakefile.h"
#include "bool.h"
#include "str.h"


struct CLARGS {
    char* directory;
    char* filename;
    bool ignore_failed_actions;
    bool dry_run;
    bool print_bakefile;
    bool silent;
} typedef CLARGS;

CLARGS parse_args(int argc, char** argv) {
    CLARGS clargs;
    // defaults
    clargs.directory = NULL;
    clargs.filename = NULL;
    clargs.ignore_failed_actions = false;
    clargs.dry_run = false;
    clargs.print_bakefile = false;
    clargs.silent = false;
    // go over every arg 
    for (int i = 0; i < argc; i++) {
        char* v = argv[i];
        if (strcmp(v, "-C") == 0) {
            clargs.directory = argv[++i];
        } else if (strcmp(v, "-f") == 0) {
            clargs.filename = argv[++i];
        } else if (strcmp(v, "-i") == 0) {
            clargs.ignore_failed_actions = true;
        } else if (strcmp(v, "-n") == 0) {
            clargs.dry_run = true;
        } else if (strcmp(v, "-p") == 0) {
            clargs.print_bakefile = true;
        } else if (strcmp(v, "-s") == 0) {
            clargs.silent = true;
        }
    }
    return clargs;
}

int main(int argc, char** argv) {
    CLARGS clargs = parse_args(argc, argv);

    if (clargs.directory != NULL) {
        chdir(clargs.directory);
    }

    BakeFile* bake = BakeFile_new(NULL);

    if (clargs.print_bakefile) {
        BakeFile_print(bake);
        exit(EXIT_SUCCESS);
    }

    BakeFile_run(bake);
    
    BakeFile_free(bake);
    return EXIT_SUCCESS;
}
