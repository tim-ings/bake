#ifndef RE_GLOBAL_H
#define RE_GLOBAL_H

#include <regex.h>


regex_t re_varexpansion;
regex_t re_variable;
regex_t re_target_nodep;
regex_t re_target_dep;
regex_t re_action;
regex_t re_url;

void re_global_comp();
void re_global_free();

#endif
