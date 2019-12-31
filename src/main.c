#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <curl/curl.h>

#include "action.h"
#include "variable.h"
#include "bakefile.h"
#include "bool.h"
#include "str.h"
#include "clargs.h"
#include "re_global.h"


bool parse_args(int argc, char** argv) {
    // defaults
    clargs.directory = NULL;
    clargs.filename = NULL;
    clargs.target = NULL;
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
        } else if (strcmp(v, "-h") == 0) {
            return false;
        } else if (strcmp(v, "--help") == 0) {
            return false;
        } else if (strcmp(v, "-?") == 0) {
            return false;
        } else if (strcmp(v, "--usage") == 0) {
            return false;
        } else if (i != 0) {
            clargs.target = argv[i];
        }
    }
    return true;
}

void print_usage() {
    printf("Usage:\n");
    printf("\tbake [-C dirname] [-f filename] [-i] [-n] [-p] [-s] [targetname]\n");
    printf("\tArguments:\n");
    printf("\t\ttargetname\tSpecify the default target. Defaults to the first target\n");
    printf("\tOptions:\n");
    printf("\t\t-C dirname\tSpecify directory to run bake from\n");
    printf("\t\t-f filename\tSpecify bakefile. Relative to -C dirname\n");
    printf("\t\t-i\t\tContinue even if actions fail\n");
    printf("\t\t-n\t\tPerform a dry run. Print actions to stdout but do not perform them\n");
    printf("\t\t-p\t\tPrint parsed bakefile\n");
    printf("\t\t-s\t\tSilently execute. Do not print any actions to stdout\n");
}

int main(int argc, char** argv) {
    if (!parse_args(argc, argv)) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    // init curl and regexes
    curl_global_init(CURL_GLOBAL_DEFAULT);
    re_global_comp();

    if (clargs.directory != NULL)
        chdir(clargs.directory);

    BakeFile* bake = BakeFile_new(clargs.filename);

    if (clargs.print_bakefile) {
        BakeFile_print(bake);
        exit(EXIT_SUCCESS);
    }

    String* target = NULL;
    if (clargs.target != NULL)
        target = String_new(clargs.target);
    BakeFile_run(bake, target);
    
    // free resources
    curl_global_cleanup();
    re_global_free();
    BakeFile_free(bake);
    return EXIT_SUCCESS;
}
