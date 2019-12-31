#include "target.h"


Target* Target_new(String* name, List* deps) {
    if (!re_url_compiled) {
        regcomp(&re_url, "^(https://)|(https://)|(file:///)", REG_EXTENDED);
        re_url_compiled = true;
    }

    Target* t = malloc(sizeof(Target));
    t->name = name;
    t->dependecies = deps;
    t->actions = List_new();
    return t;
}

void Target_free(Target* self) {
    for (int i = 0; i < self->actions->length; i++) {
        Action_free(List_get(self->actions, i));
    }
    List_free(self->actions);
    for (int i = 0; i < self->dependecies->length; i++) {
        String_free(List_get(self->dependecies, i));
    }
    List_free(self->dependecies);
    String_free(self->name);
    free(self);
}

bool Target_eq(void* a, void* b) {
    Target* tar1 = (Target*)a;
    String* tar2_name = (String*)b;
    return String_equals(tar1->name, tar2_name);
}

void Target_addAction(Target* self, Action* action) {
    List_add(self->actions, action);
}

long url_modified(String* url) {
    // set up curl
    CURL* curl;
    curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "FATAL: Failed to init curl in url_modified\n");
        exit(EXIT_FAILURE);
    }
    // ask for file time header only
    curl_easy_setopt(curl, CURLOPT_URL, url->str);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_FILETIME, 1L);
    curl_easy_setopt(curl, CURLOPT_HEADER, 0L);

    CURLcode res;
    long filetime = -1;
    res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        res = curl_easy_getinfo(curl, CURLINFO_FILETIME, &filetime);
        if (res != CURLE_OK || filetime < 0)
            filetime = -1;
    }
    curl_easy_cleanup(curl);
    return filetime;
}

bool Target_isOutDated(Target* self, List* others) {
    // If there are no dependencies, then the target requires rebuilding
    if (self->dependecies->length == 0) {
        return true;
    }

    // If any dependency does not exist, or has been modified more recently 
    // than its target, then the target requires rebuilding

    // check every dependency
    for (int di = 0; di < self->dependecies->length; di++) {
        String* dep = List_get(self->dependecies, di);
        
        // check if the dependency is another target
        Target* other = List_find(others, Target_eq, dep);
        if (other != NULL && Target_isOutDated(other, others)) {
            // build the outdated dependency
            Target_build(other);
            return true;
        }

        // check if the dependency is a url
        List* matches = String_match(dep, &re_url, 1);
        if (matches != NULL) {
            // check if the target file exists
            if (access(self->name->str, F_OK) != -1) {
                long modified = url_modified(dep); // get url time modified
                struct stat tarstats;
                stat(self->name->str, &tarstats); // get file time modified
                if (modified > tarstats.st_mtime) {
                    printf("target is outdated due to URL %ld > %ld\n", modified, tarstats.st_mtime);
                    return true;
                }
            }
            // free matches
            for (int i = 0; i < matches->length; i++)
                ReMatch_free(List_get(matches, i));
            List_free(matches);
        }

        // check if the target file exists
        if (access(self->name->str, F_OK) == -1)
            return true;

        // check if the dependant file exists
        if (access(dep->str, F_OK) == -1)
            return true;

        // check if the dependant file has been modified more recently than the target file
        struct stat depstats;
        struct stat tarstats;
        stat(dep->str, &depstats);
        stat(self->name->str, &tarstats);
        if (depstats.st_mtime > tarstats.st_mtime)
            return true;
    }

    return false;
}

int Target_build(Target* self) {
    // run all actions for this target
    for (int ai = 0; ai < self->actions->length; ai++) {
        Action* act = List_get(self->actions, ai);
        if (!clargs.silent && act->modifier != '@') // supress output with '@'
            printf("%s\n", act->command->str);
        if (!clargs.dry_run) {
            int res = Action_exec(act);
            if (res != 0)
                return res; // stop executing actions if one fails (non-zero)
        }
    }
    return 0;
}
