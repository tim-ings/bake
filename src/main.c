#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "action.h"
#include "variable.h"
#include "bakefile.h"
#include "bool.h"
#include "str.h"


int main(int argc, char** argv) {
    // parse the bakefile
    BakeFile* bake = BakeFile_new(NULL);
    BakeFile_print(bake);

    // run the bakefile
    BakeFile_run(bake);
    
    // free resources and exit
    BakeFile_free(bake);
    regfree(&re_variable);
    regfree(&re_target_nodep);
    regfree(&re_target_dep);
    regfree(&re_action);
    return EXIT_SUCCESS;
}
