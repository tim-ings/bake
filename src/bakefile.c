#include "bakefile.h"


BakeFile* BakeFile_new() {
    BakeFile* bake = malloc(sizeof(BakeFile));
    bake->variables_len = 0;
    bake->targets_len = 0;
    bake->variables = malloc(sizeof(Variable*) * BUFSIZ);
    bake->targets = malloc(sizeof(Variable*) * BUFSIZ);

    // set up regex here as static var causes segfaults
    regcomp(&re_varexpansion, "\\$\\(([^\\(\\) \t]+)\\)", REG_EXTENDED);

    return bake;
}

void BakeFile_free(BakeFile self) {
    for (int i = 0; i < self.variables_len; i++) {
        Variable_free(self.variables[i]);
    }
    free(self.variables);

    for (int i = 0; i < self.targets_len; i++) {
        Target_free(self.targets[i]);
    }
    free(self.targets);
}

char* BakeFile_getVar(BakeFile* self, char* name) {
    // check for special cases
    if (strcmp(name, "PID") == 0) {
        char* val = malloc(sizeof(char*) * BUFSIZ);
        sprintf(val, "%d", getpid());
        return val;
    }
    if (strcmp(name, "PPID") == 0) { 
        char* val = malloc(sizeof(char*) * BUFSIZ);
        sprintf(val, "%d", getppid());
        return val;    
    }
    if (strcmp(name, "PWD") == 0) { 
        char* cwd = malloc(sizeof(char) * BUFSIZ);
        getcwd(cwd, BUFSIZ);
        return cwd;
    }
    if (strcmp(name, "RAND") == 0) { 
        char* val = malloc(sizeof(char*) * BUFSIZ);
        sprintf(val, "%d", rand());
        return val;
    }

    // try find and the var in our var list
    for (int i = 0; i < self->variables_len; i++) {
        Variable* var = &self->variables[i];
        if (strcmp(var->name, name) == 0) {
            return var->value;
        }
    }

    // if we didnt find it try find in env
    char* val = getenv(name);
    if (val != NULL) { return val; }

    // finally default to the empty string
    return "";
}

void BakeFile_setVar(BakeFile* self, char* name, char* value) {
    // expand the value
    value = BakeFile_varExpand(self, &value);

    // try updating an existing variable
    for (int i = 0; i < self->variables_len; i++) {
        Variable* var = &self->variables[i];
        if (strcmp(var->name, name) == 0) {
            var->value = value;
            return;
        }
    }

    // no existing variable, so make a new one
    self->variables_len++;
    Variable* vars = realloc(self->variables, sizeof(Variable) * self->variables_len);
    if (!vars) {
        fprintf(stderr, "FATAL: Realloc failed in BakeFile_setVar\n");
        exit(EXIT_FAILURE);
    }
    self->variables = vars; // update the pointer after realloc

    // set up our new variable
    Variable* var = &self->variables[self->variables_len - 1];
    var->name = name;
    var->value = value;
}

void BakeFile_print(BakeFile* self) {
    printf("Variables:\n");
    for (int i = 0; i < self->variables_len; i++) {
        Variable* v = &self->variables[i];
        printf("\t%s = \"%s\"\n", v->name, v->value);
    }
    printf("Targets:\n\tNYI\n");
    for (int i = 0; i < self->targets_len; i++) {
        // TODO: after impl targets
        //Target* t = &self->targets[i];
        //printf("\t%s = \"%s\"\n", t->name, t->value);
    }
}

char* BakeFile_varExpand(BakeFile* self, char** strp) {
    // find the next var to expand
    char* str = *strp;
    size_t numMatches = 2;
    regmatch_t matches[numMatches];
    if (regexec(&re_varexpansion, str, numMatches, matches, 0) != REG_NOMATCH) {
        // get the name of the variable we want to expand
        char* name = malloc(sizeof(char) * matches[1].rm_eo - matches[1].rm_so);
        strncpy(name, &str[matches[1].rm_so], matches[1].rm_eo - matches[1].rm_so);

        // try get the variable from our bakefile
        char* val = BakeFile_getVar(self, name);

        // get the string before the expansion
        size_t before_len = matches[1].rm_so - 2; // take 2 for for "$(" chars
        char* before = malloc(sizeof(char) * before_len);
        strncpy(before, str, before_len);

        // get the string after the expansion
        size_t str_len = strlen(str);
        size_t after_len = str_len - matches[1].rm_eo - 1; // take 1 for ")" char
        char* after = malloc(sizeof(char) * after_len);
        strncpy(after, &str[matches[1].rm_eo + 1], after_len);

        // make the expanded string
        size_t val_len = strlen(val);
        char* expstr = realloc(str, sizeof(char) * (before_len + val_len + after_len)); // take 3 for "$()" chars
        if (!expstr) { 
            fprintf(stderr, "FATAL: Realloc failed in BakeFile_expand\n");
            exit(EXIT_FAILURE); 
        } // failed to realloc
        *strp = expstr; // update str pointer and str
        str = *strp;
        strncpy(str, before, before_len);
        strncpy(&str[before_len], val, val_len);
        strncpy(&str[before_len + val_len], after, after_len);
        strncpy(&str[before_len + val_len + after_len], "\0", 1); // terminate the string
        // clean up
        free(after);
        free(before);
        free(name);
        // keep checking for more variables
        return BakeFile_varExpand(self, strp); 
    } else {
        return str; // base case when no matches are found
    }
}
