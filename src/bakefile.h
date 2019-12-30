#ifndef H_BAKEFILE
#define H_BAKEFILE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>

#include "variable.h"
#include "target.h"


regex_t re_varexpansion;

struct BakeFile
{
    Variable* variables;
    size_t variables_len;
    Target* targets;
    size_t targets_len;
} typedef BakeFile;

BakeFile* BakeFile_new();
void BakeFile_free(BakeFile self);
char* BakeFile_getVar(BakeFile* self, char* name);
void BakeFile_setVar(BakeFile* self, char* name, char* value);
void BakeFile_print(BakeFile* self);
char* BakeFile_varExpand(BakeFile* self, char** strp);
void BakeFile_addTarget(BakeFile* self, Target* target);
void BakeFile_run(BakeFile* bake);

#endif