#ifndef H_BAKEFILE
#define H_BAKEFILE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>

#include "variable.h"
#include "target.h"
#include "str.h"
#include "bool.h"
#include "re_global.h"


struct BakeFile
{
    List* variables;
    List* targets;
    String* default_target;
} typedef BakeFile;

BakeFile* BakeFile_new(char* file_path);
void BakeFile_free(BakeFile* self);
String* BakeFile_getVar(BakeFile* self, String* name);
void BakeFile_setVar(BakeFile* self, String* name, String* value);
void BakeFile_print(BakeFile* self);
String* BakeFile_varExpand(BakeFile* self, String* str);
void BakeFile_addTarget(BakeFile* self, Target* target);
void BakeFile_run(BakeFile* bake, String* target);

#endif
