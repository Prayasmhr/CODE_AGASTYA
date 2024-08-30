#ifndef _TJLANG_LEXER_H 
#define _TJLANG_LEXER_H 

#include"module.h"

typedef struct tjlang_lexer_state tjlang_lexer_state;

void _lexer_init(const void* file,void** lexer_process_state);
void* _lexer_drive(const void* _file,void* lexer_process_state);
void _lexer_pass_data_handle(void* _data,void* lexer_process_state);
void _lexer_reset(const void* _data,void* lexer_process_state); 
static bool _lexer_drive_internal(tjlang_lexer_state* lexer_state);
static void _lexer_advance_with_schar(tjlang_lexer_state* lexer_state,tjlang_central_enum token_type, bool with_identifier);
static void _lexer_advance_with_operator(tjlang_lexer_state* lexer_state);
static void _lexer_advance_with_colon(tjlang_lexer_state* lexer_state);
static void _lexer_advance_with_string(tjlang_lexer_state* lexer_state);
static void _lexer_advance_with_identifier(tjlang_lexer_state* lexer_state);
static void _lexer_advance_with_uientt_typespec(tjlang_lexer_state* lexer_state);
void _lexer_cleanup(void* token_data,void* lexer_process_state);
void _lexer_close(void* token_data,void* lexer_process_state);

#endif
