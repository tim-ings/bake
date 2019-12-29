#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define bool char
#define true 1
#define false 0

struct Action
{
    char modifier;
    char* command;
} typedef Action;

struct Variable
{
    char* name;
    char* value;
} typedef Variable;

struct Target
{
    char* name;
    char** dependecies;
    Action* actions;
    size_t actions_len;
} typedef Target;

struct BakeFile
{
    Variable* variables;
    size_t variables_len;
    Target* targets;
    size_t targets_len;
} typedef BakeFile;

BakeFile* BakeFile_new() {
    BakeFile* bake = malloc(sizeof(BakeFile));
    bake->variables_len = 0;
    bake->targets_len = 0;
    bake->variables = malloc(sizeof(Variable*) * BUFSIZ);
    bake->targets = malloc(sizeof(Variable*) * BUFSIZ);
    return bake;
}

void BakeFile_print(BakeFile* self) {
    printf("Variables:\n");
    for (int i = 0; i < self->variables_len; i++) {
        Variable* v = &self->variables[i];
        printf("\t%s = \"%s\"\n", v->name, v->value);
    }
    printf("Targets:\n");
    for (int i = 0; i < self->targets_len; i++) {
        // TODO: after impl targets
        //Variable* v = &self->variables[i];
        //printf("\t%s = \"%s\"\n", v->name, v->value);
    }
}

regex_t re_variable;
regex_t re_varexpansion;
regex_t re_target;
regex_t re_action;

bool setVariable(BakeFile* bake, char* name, char* value) {
    // try updating an exisitng variable with the same name
    for (int i = 0; i < bake->variables_len; i++) {
        Variable* var = &bake->variables[i];
        if (strcmp(var->name, name) == 0) {
            var->value = value;
            return true;
        }
    }
    // we did not find an existing variable so make a new one
    bake->variables_len++;
    Variable* vars = realloc(bake->variables, sizeof(Variable) * bake->variables_len);
    if (!vars) {
        return false;
    }
    bake->variables = vars; // update the pointer after realloc
    Variable* var = &bake->variables[bake->variables_len - 1];
    var->name = name;
    var->value = value;
    return true;
}

char* expandVariables(BakeFile* bake, char** str_p) {
    char* str = *str_p;
    // check for variable expansion in str
    size_t numMatches = 2;
    regmatch_t matches[numMatches];
    // find the next var to expand
    if (regexec(&re_varexpansion, str, numMatches, matches, 0) != REG_NOMATCH) {
        // get the name of the variable we want to expand
        char* name = malloc(sizeof(char) * matches[1].rm_eo - matches[1].rm_so);
        strncpy(name, &str[matches[1].rm_so], matches[1].rm_eo - matches[1].rm_so);

        // find the value of the variable
        char* val = NULL;
        for (int vi = 0; vi < bake->variables_len; vi++) {
            if (strcmp(bake->variables[vi].name, name) == 0) { // found
                val = bake->variables[vi].value;
                break;
            }
        }
        // exit if we did not find it
        if (val == NULL) {
            printf("Failed to find variable: \"%s\"", name);
            exit(EXIT_FAILURE);
        }

        // get the string before the expansion
        size_t before_len = matches[1].rm_so - 2; // take 2 for for $( chars
        char* before = malloc(sizeof(char) * before_len);
        strncpy(before, str, before_len);

        // get the string after the expansion
        size_t str_len = strlen(str);
        size_t after_len = str_len - matches[1].rm_eo - 1; // take 1 for ) char
        char* after = malloc(sizeof(char) * after_len);
        strncpy(after, &str[matches[1].rm_eo + 1], after_len);

        // make the expanded string
        size_t val_len = strlen(val);
        char* expstr = realloc(str, sizeof(char) * (before_len + val_len + after_len)); // take 3 for $() chars
        if (!expstr) { exit(EXIT_FAILURE); } // failed to realloc
        *str_p = expstr; // update str pointer
        str = *str_p; // and str
        strncpy(str, before, strlen(before));
        strncpy(&str[before_len], val, val_len);
        strncpy(&str[before_len + val_len], after, after_len);
        strncpy(&str[before_len + val_len + after_len], "\0", 1); // terminate the string
        // clean up
        free(after);
        free(before);
        free(name);
        // keep checking for more variables
        return expandVariables(bake, str_p); 
    } else {
        return str; // base case when no matches are found
    }
}

bool parseFile(char* filePath, BakeFile* outBake) {
    // open the file
    if (!filePath) {
        filePath = "bakefile";
    }
    FILE* fp = fopen(filePath, "r");
    if (fp == NULL)
        return false; // we failed to open the file

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
            // expand any variables in the value
            expandVariables(outBake, &value);
            // save the variable to our bakefile struct
            if (!setVariable(outBake, name, value)) {
                return false; // failed to allocate variable
            }
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
    return true; // we parsed the file
}

bool init() {
    // NAME = VALUE
    if (regcomp(&re_variable, "^([^= \t]+)[ \t]*=[ \t]*(.+)*$", REG_EXTENDED)) return false;
    // $(NAME)
    if (regcomp(&re_varexpansion, "\\$\\(([^\\(\\) \t]+)\\)", REG_EXTENDED)) return false;
    // target: deps
    if (regcomp(&re_target, "^[^\t ](.+)[ \t]*:[ \t]*(.+)$", REG_EXTENDED)) return false;
    // \tcommand
    if (regcomp(&re_action, "^[\t](.+)$", REG_EXTENDED)) return false;

    return true;
}

int final() {

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    // initialise the program, build regexs etc.
    if (!init()) {
        printf("Failed to initialise bake");
        exit(EXIT_FAILURE);
    }

    // parse the bakefile
    BakeFile* bake = BakeFile_new();
    if (!parseFile(NULL, bake)) {
        exit(EXIT_FAILURE);
    }
    BakeFile_print(bake);

    // run the bakefile
    
    // free resources and exit
    return final();
}