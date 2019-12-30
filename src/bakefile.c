#include "bakefile.h"


BakeFile* BakeFile_new(char* file_path) {
    BakeFile* bake = malloc(sizeof(BakeFile));
    bake->variables = List_new();
    bake->targets = List_new();

    // set up regex here var causes segfaults
    regcomp(&re_varexpansion, "\\$\\(([^\\(\\) \t]+)\\)", REG_EXTENDED);
    regcomp(&re_variable, "^([^= \t]+)[ \t]*=[ \t]*(.+)*$", REG_EXTENDED);
    regcomp(&re_target_nodep, "^([^: \t ]+)[ \t]*:[ \t]*$", REG_EXTENDED);
    regcomp(&re_target_dep, "^([^: \t ]+)[ \t]*:[ \t]*(.+)$", REG_EXTENDED);
    regcomp(&re_action, "^[\t](.+)$", REG_EXTENDED);

    // parse the bakefile
    // open the file
    if (file_path == NULL)
        file_path = "bakefile";
    FILE* fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("Failed to open file: %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    // read the file line by line
    char* line = NULL;
    String* line_str = String_new("");
    ssize_t read;
    size_t len = 0;
    Target* current_target;
    while ((read = getline(&line, &len, fp)) != EOF) {
        strtok(line, "\n"); // strip new line
        if (line[0] == '#') continue; // ignore comments
        line_str->str = line; // set up String object with new line

        List* matches = NULL;

        // check for variables
        matches = String_match(line_str, &re_variable, 3);
        if (matches != NULL) {
            String* name = String_copy(((ReMatch*)List_get(matches, 1))->match);
            String* value = String_copy(((ReMatch*)List_get(matches, 2))->match);
            BakeFile_setVar(bake, name, value);

            for (int i = 0; i < matches->length; i++)
                ReMatch_free(List_get(matches, i));
            List_free(matches);
            continue;
        }

        // check for targets with no dependencies
        matches = String_match(line_str, &re_target_nodep, 2);
        if (matches != NULL) {
            // we have found a new target so add the old one to our bakefile
            if (current_target != NULL) BakeFile_addTarget(bake, current_target);
            // make a new target
            String* name = String_copy(((ReMatch*)List_get(matches, 1))->match);
            name = BakeFile_varExpand(bake, name);
            current_target = Target_new(name, List_new());

            for (int i = 0; i < matches->length; i++)
                ReMatch_free(List_get(matches, i));
            List_free(matches);
            continue;
        }

        // check for targets with dependencies
        matches = String_match(line_str, &re_target_dep, 3);
        if (matches != NULL) {
            // we have found a new target so add the old one to our bakefile
            if (current_target != NULL) BakeFile_addTarget(bake, current_target);
            // make a new target
            String* name = String_copy(((ReMatch*)List_get(matches, 1))->match);
            name = BakeFile_varExpand(bake, name);
            String* deps_str = String_copy(((ReMatch*)List_get(matches, 2))->match);
            List* deps = String_split(deps_str, ' ');
            current_target = Target_new(name, deps);
            
            for (int i = 0; i < matches->length; i++)
                ReMatch_free(List_get(matches, i));
            List_free(matches);
            continue;
        }

        // check for actions
        matches = String_match(line_str, &re_action, 2);
        if (matches != NULL) {
            String* command = String_copy(((ReMatch*)List_get(matches, 1))->match);
            char mod = 0;
            switch (command->str[0])
            {
                case '@': mod = '@'; break;
                case '-': mod = '-'; break;
            }
            command = BakeFile_varExpand(bake, command);
            Action* action = Action_new(mod, command);
            Target_addAction(current_target, action);
            
            for (int i = 0; i < matches->length; i++)
                ReMatch_free(List_get(matches, i));
            List_free(matches);
            continue;
        }
    }

    // add our final target to the bakefile
    if (current_target != NULL) {
        BakeFile_addTarget(bake, current_target);
    }

    // free memory
    fclose(fp);
    if (line)
        free(line);

    return bake;
}

void BakeFile_free(BakeFile* self) {
    for (int i = 0; i < self->variables->length; i++) {
        Variable_free(List_get(self->variables, i));
    }
    List_free(self->variables);

    for (int i = 0; i < self->targets->length; i++) {
        Target_free(List_get(self->targets, i));
    }
    List_free(self->targets);

    regfree(&re_varexpansion);
    regfree(&re_variable);
    regfree(&re_target_nodep);
    regfree(&re_target_dep);
    regfree(&re_action);

    free(self);
}

String* BakeFile_getVar(BakeFile* self, String* name) {
    // check for special cases
    if (String_equals_c(name, "PID")) {
        return String_from_int(getpid());
    }
    if (String_equals_c(name, "PPID")) { 
        return String_from_int(getppid());
    }
    if (String_equals_c(name, "PWD")) { 
        char* cwd = malloc(sizeof(char) * BUFSIZ);
        getcwd(cwd, BUFSIZ);
        return String_new(cwd);
    }
    if (String_equals_c(name, "RAND")) { 
        return String_from_int(rand());
    }

    // try find and the var in our var list
    for (int i = 0; i < self->variables->length; i++) {
        Variable* var = List_get(self->variables, i);
        if (String_equals(var->name, name)) {
            return var->value;
        }
    }

    // if we didnt find it try find in env
    char* val = getenv(name->str);
    if (val != NULL) { return String_new(val); }

    // finally default to the empty string
    return String_new("");
}

void BakeFile_setVar(BakeFile* self, String* name, String* value) {
    // expand the value
    value = BakeFile_varExpand(self, value);

    // try updating an existing variable
    for (int i = 0; i < self->variables->length; i++) {
        Variable* var = List_get(self->variables, i);
        if (String_equals(var->name, name)) {
            var->value = value;
            return;
        }
    }

    // set up our new variable
    Variable* var = Variable_new(name, value);
    List_add(self->variables, var);
}

void BakeFile_print(BakeFile* self) {
    printf("Variables:\n");
    for (int i = 0; i < self->variables->length; i++) {
        Variable* v = List_get(self->variables, i);
        printf("\t%s = \"%s\"\n", v->name->str, v->value->str);
    }
    printf("Targets:\n");
    for (int i = 0; i < self->targets->length; i++) {
        Target* t = List_get(self->targets, i);
        printf("\t%s:", t->name->str);
        for (int di = 0; di < t->dependecies->length; di++) {
            printf("\t %s", ((String*)List_get(t->dependecies, di))->str);
        }
        printf("\n");
        for (int ai = 0; ai < t->actions->length; ai++) {
            Action* act = List_get(t->actions, ai);
            char mod = act->modifier == '\0' ? ' ' : act->modifier;
            printf("\t\t[%c] \"%s\"\n", mod, act->command->str);
        }
    }
}

String* BakeFile_varExpand(BakeFile* self, String* str) {
    List* matches = String_match(str, &re_varexpansion, 2);
    if (matches != NULL) {
        ReMatch* match = List_get(matches, 1);
        String* before = String_slice(str, 0, match->start - 2);
        String* after = String_slice(str, match->end + 1, str->length);
        String* val = BakeFile_getVar(self, match->match);
        String* expanded = String_concat(String_concat(before, val), after);
        expanded->str[expanded->length] = '\0';

        for (int i = 0; i < matches->length; i++)
            ReMatch_free(List_get(matches, i));
        List_free(matches);
        return BakeFile_varExpand(self, expanded);
    }
    
    return str;
}

void BakeFile_addTarget(BakeFile* self, Target* target) {
    List_add(self->targets, target);
}

void BakeFile_run(BakeFile* self) {
    // // rebuild dated targets
    // for (int ti = 0; ti < self->targets->length; ti++) {
    //     Target* t = List_get(self->targets, ti);
    //     printf("Checking target: %s\n", t->name);
    //     if (Target_isOutDated(t, self->targets)) {
    //         Target_build(t);
    //     }
    // }
}
