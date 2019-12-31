#ifndef H_CLARGS
#define H_CLARGS

struct CLARGS {
    char* directory;
    char* filename;
    char* target;
    bool ignore_failed_actions;
    bool dry_run;
    bool print_bakefile;
    bool silent;
} typedef CLARGS;

CLARGS clargs;

#endif
