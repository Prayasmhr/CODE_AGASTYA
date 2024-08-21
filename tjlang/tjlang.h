#ifndef _TJLANG_H
#define _TJLANG_H

#include"module.h"
#include"interpreter.h"
#include"lexer.h"
#include"parser.h"

void tjlang_interpret(char* app_module_name,char* file_name);
static void tjlang_lexer(void);
static void tjlang_parser(void);
static void tjlang_interpreter(void);

#endif
