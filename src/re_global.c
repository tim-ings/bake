#include "re_global.h"


void re_global_comp() {
    regcomp(&re_varexpansion, "\\$\\(([^\\(\\) \t]+)\\)", REG_EXTENDED);
    regcomp(&re_variable, "^([^= \t]+)[ \t]*=[ \t]*(.+)*$", REG_EXTENDED);
    regcomp(&re_target_nodep, "^([^: \t ]+)[ \t]*:[ \t]*$", REG_EXTENDED);
    regcomp(&re_target_dep, "^([^: \t ]+)[ \t]*:[ \t]*(.+)$", REG_EXTENDED);
    regcomp(&re_action, "^[\t](.+)$", REG_EXTENDED);
    regcomp(&re_url, "^(https://)|(https://)|(file:///)", REG_EXTENDED);
}

void re_global_free() {
    regfree(&re_varexpansion);
    regfree(&re_variable);
    regfree(&re_target_nodep);
    regfree(&re_target_dep);
    regfree(&re_action);
    regfree(&re_url);
}
