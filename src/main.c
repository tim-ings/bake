#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "action.h"
#include "variable.h"
#include "bakefile.h"

#define bool char
#define true 1
#define false 0

regex_t re_variable;
regex_t re_target_nodep;
regex_t re_target_dep;
regex_t re_action;

void parseFile(char* filePath, BakeFile* outBake) {
    // open the file
    if (!filePath) {
        filePath = "bakefile";
    }
    FILE* fp = fopen(filePath, "r");
    if (fp == NULL) {
        printf("Failed to open file: %s\n", filePath);
        exit(EXIT_FAILURE);
    }

    // read the file line by line
    char * line = NULL;
    ssize_t read;
    size_t len = 0;
    Target* current_target;
    while ((read = getline(&line, &len, fp)) != EOF) {
        strtok(line, "\n"); // strip new line
        // printf("PARSING \"%s\"\n", line);
        // try parse as a variable definition
        size_t numMatches = 4;
        regmatch_t matches[numMatches];
        if (regexec(&re_variable, line, numMatches, matches, 0) != REG_NOMATCH) {
            char* name = malloc(sizeof(char) * matches[1].rm_eo - matches[1].rm_so); // this becomes owned by a variable struct so dont free
            strncpy(name, &line[matches[1].rm_so], matches[1].rm_eo - matches[1].rm_so);
            char* value = malloc(sizeof(char) * matches[2].rm_eo - matches[2].rm_so); // this becomes owned by a variable struct so dont free
            strncpy(value, &line[matches[2].rm_so], matches[2].rm_eo - matches[1].rm_so);
            BakeFile_setVar(outBake, name, value);
            // printf("\tSET \"%s\" to \"%s\"\n", name, value);
            continue;
        }
        // try parse as a target with no dependencies
        if (regexec(&re_target_nodep, line, numMatches, matches, 0) != REG_NOMATCH) {
            // we have found a new target so add the old one to our bakefile
            if (current_target != NULL) {
                BakeFile_addTarget(outBake, current_target);
            }
            // make a new target
            char* name = malloc(sizeof(char) * matches[1].rm_eo - matches[1].rm_so); // this becomes owned by a target struct so dont free
            strncpy(name, &line[matches[1].rm_so], matches[1].rm_eo - matches[1].rm_so);
            name = BakeFile_varExpand(outBake, &name);
            current_target = Target_new(name, NULL, 0);
            continue;
        }
        // try parse as a target with dependencies
        if (regexec(&re_target_dep, line, numMatches, matches, 0) != REG_NOMATCH) {
            // we have found a new target so add the old one to our bakefile
            if (current_target != NULL) {
                BakeFile_addTarget(outBake, current_target);
            }
            // make a new target
            char* name = malloc(sizeof(char) * matches[1].rm_eo - matches[1].rm_so); // this becomes owned by a target struct so dont free
            strncpy(name, &line[matches[1].rm_so], matches[1].rm_eo - matches[1].rm_so);
            name = BakeFile_varExpand(outBake, &name);
            
            char* deps = malloc(sizeof(char) * matches[2].rm_eo - matches[2].rm_so); // this becomes owned by a variable struct so dont free
            strncpy(deps, &line[matches[2].rm_so], matches[2].rm_eo - matches[1].rm_so);
            deps = BakeFile_varExpand(outBake, &deps);

            // build a dependency list
            char** deplist = malloc(sizeof(char*) * BUFSIZ);
            int depcount = 0;
            const char delim[2] = " ";
            char* tok = strtok(deps, delim);
            while (tok != NULL) {
                deplist[depcount++] = strdup(tok);
                tok = strtok(NULL, delim);
            }
            // make a new target
            current_target = Target_new(name, deplist, depcount);
            continue;
        }
        // try parse as action
        if (regexec(&re_action, line, numMatches, matches, 0) != REG_NOMATCH) {
            printf("Found an action: %s\n", line);
            char* command;
            char mod = 0;
            if (line[1] == '@' || line[1] == '-') {
                mod = line[1];
                command = malloc(sizeof(char) * strlen(line) - 2);
                strncpy(command, &line[2], strlen(line) - 2);
            } else {
                command = malloc(sizeof(char) * strlen(line) - 1);
                strncpy(command, &line[1], strlen(line) - 1);
            }
            command = BakeFile_varExpand(outBake, &command);
            Action* action = Action_new(mod, command);
            printf("Adding an action with mod: %c, and command: %s\n", action->modifier, action->command);
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
    
    // free resources and exit
    BakeFile_free(*bake);
    free(bake);
    regfree(&re_variable);
    regfree(&re_target_nodep);
    regfree(&re_target_dep);
    regfree(&re_action);
    return EXIT_SUCCESS;
}