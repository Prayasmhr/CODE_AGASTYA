#ifndef _TJLANG_H
#define _TJLANG_H

#include"module.h"
#include"interpreter.h"
#include"lexer.h"
#include"parser.h"

typedef struct tjlang_startup_info {
    char* file_name;
    char* write_name;
    u32 interpret_count;
} tjlang_startup_info;

void tjlang_interpret(char* app_module_name,char* file_name,char* script_module_file);
static void tjlang_lexer(void);
static void tjlang_parser(void);
static void tjlang_interpreter(void);

#endif
