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
regex_t re_varexpansion;
regex_t re_target;
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
    while ((read = getline(&line, &len, fp)) != -1) {
        strtok(line, "\n"); // strip new line
        printf("PARSING \"%s\"\n", line);
        // try parse as a variable definition
        size_t numMatches = 4;
        regmatch_t matches[numMatches];
        if (regexec(&re_variable, line, numMatches, matches, 0) != REG_NOMATCH) {
            char* name = malloc(sizeof(char) * matches[1].rm_eo - matches[1].rm_so); // this becomes owned by a variable struct so dont free
            strncpy(name, &line[matches[1].rm_so], matches[1].rm_eo - matches[1].rm_so);
            char* value = malloc(sizeof(char) * matches[2].rm_eo - matches[2].rm_so); // this becomes owned by a variable struct so dont free
            strncpy(value, &line[matches[2].rm_so], matches[2].rm_eo - matches[1].rm_so);
            BakeFile_setVar(outBake, name, value);
            printf("\tSET \"%s\" to \"%s\"\n", name, value);
            continue;
        }
        if (regexec(&re_target, line, numMatches, matches, 0) != REG_NOMATCH) {
            
        }
    }

    // free memory
    fclose(fp);
    if (line)
        free(line);
}

bool init() {
    if (regcomp(&re_variable, "^([^= \t]+)[ \t]*=[ \t]*(.+)*$", REG_EXTENDED)) return false;
    if (regcomp(&re_target, "^[^\t ](.+)[ \t]*:[ \t]*(.+)$", REG_EXTENDED)) return false;
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
    return EXIT_SUCCESS;
}