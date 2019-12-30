#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "action.h"
#include "variable.h"
#include "bakefile.h"
#include "bool.h"
#include "str.h"


regex_t re_variable;
regex_t re_target_nodep;
regex_t re_target_dep;
regex_t re_action;

void parseFile(String* file_path, BakeFile* outBake) {
    // open the file
    if (file_path == NULL) file_path = String_new("bakefile");
    FILE* fp = fopen(file_path->str, "r");
    if (fp == NULL) {
        printf("Failed to open file: %s\n", file_path->str);
        exit(EXIT_FAILURE);
    }

    // read the file line by line
    char* line = NULL;
    ssize_t read;
    size_t len = 0;
    Target* current_target;
    while ((read = getline(&line, &len, fp)) != EOF) {
        strtok(line, "\n"); // strip new line
        if (line[0] == '#') continue; // ignore comments

        String* line_str = String_new(line);
        List* matches = NULL;

        // check for variables
        matches = String_match(line_str, &re_variable, 3);
        if (matches != NULL) {
            String* name = ((ReMatch*)List_get(matches, 1))->match;
            String* value = ((ReMatch*)List_get(matches, 2))->match;
            BakeFile_setVar(outBake, name, value);
            continue;
        }

        // check for targets with no dependencies
        matches = String_match(line_str, &re_target_nodep, 2);
        if (matches != NULL) {
            // we have found a new target so add the old one to our bakefile
            if (current_target != NULL) BakeFile_addTarget(outBake, current_target);
            // make a new target
            String* name = ((ReMatch*)List_get(matches, 1))->match;
            name = BakeFile_varExpand(outBake, name);
            current_target = Target_new(name, List_new());
            continue;
        }

        // check for targets with dependencies
        matches = String_match(line_str, &re_target_dep, 3);
        if (matches != NULL) {
            // we have found a new target so add the old one to our bakefile
            if (current_target != NULL) BakeFile_addTarget(outBake, current_target);
            // make a new target
            String* name = ((ReMatch*)List_get(matches, 1))->match;
            name = BakeFile_varExpand(outBake, name);
            String* deps_str = ((ReMatch*)List_get(matches, 2))->match;
            List* deps = String_split(deps_str, ' ');
            current_target = Target_new(name, deps);
            continue;
        }

        // check for actions
        matches = String_match(line_str, &re_action, 2);
        if (matches != NULL) {
            String* command = ((ReMatch*)List_get(matches, 1))->match;
            char mod = 0;
            switch (command->str[0])
            {
                case '@': mod = '@'; break;
                case '-': mod = '-'; break;
            }
            command = BakeFile_varExpand(outBake, command);
            Action* action = Action_new(mod, command);
            Target_addAction(current_target, action);
        }
    }

    // add our final target to the bakefile
    if (current_target != NULL) {
        BakeFile_addTarget(outBake, current_target);
    }

    // free memory
    fclose(fp);
    if (line)
        free(line);
}

bool init() {
    if (regcomp(&re_variable, "^([^= \t]+)[ \t]*=[ \t]*(.+)*$", REG_EXTENDED)) return false;
    if (regcomp(&re_target_nodep, "^([^: \t ]+)[ \t]*:[ \t]*$", REG_EXTENDED)) return false;
    if (regcomp(&re_target_dep, "^([^: \t ]+)[ \t]*:[ \t]*(.+)$", REG_EXTENDED)) return false;
    if (regcomp(&re_action, "^[\t](.+)$", REG_EXTENDED)) return false;
    return true;
}

int main(int argc, char** argv) {
    // initialise the program, build regexs etc.
    if (!init()) {
        fprintf(stderr, "FATAL: Failed to initialise bake");
        exit(EXIT_FAILURE);
    }

    // parse the bakefile
    BakeFile* bake = BakeFile_new();
    parseFile(NULL, bake);
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
