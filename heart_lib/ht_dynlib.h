#ifndef _HT_DYNLIB_H
#define _HT_DYNLIB_H

#include"ht_defines.h"
#include<windows.h>

typedef struct dynlib dynlib; 

dynlib* dynlib_load(char* path);
FARPROC dynlib_symbol(const dynlib* lib,char* symbol);
void dynlib_unload(dynlib* lib);

#endif
