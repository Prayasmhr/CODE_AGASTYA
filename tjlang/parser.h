#ifndef _TJLANG_PARSER_H
#define _TJLANG_PARSER_H

#include"module.h"
#include"instruction_representation.h"

typedef struct tjlang_parser_state tjlang_parser_state;

void _parser_init(const void* data_module,void** parser_process_state);
void* _parser_process(const void* token_data,void* parser_process_state);
void _parser_reset(const void* token_data,void* parser_process_state);
static void _parser_process_internal(tjlang_parser_state* parser_state);
static void _parser_handle_keyword(tjlang_parser_state* parser_state);
static void _parser_acquire_uientt_prop_assignments(tjlang_parser_state* parser_state,tjlang_ir_zone* uientt_def_zone);
static void _parser_acquire_image_specs(tjlang_parser_state* parser_state,tjlang_ir_zone* asset_load_zone);
static void _parser_acquire_font_specs(tjlang_parser_state* parser_state,tjlang_ir_zone* asset_load_zone);
static tjlang_ir_instruction _parser_handle_uientt_prop_assign(tjlang_parser_state* parser_state);
static void _parser_handle_asset_def(tjlang_parser_state* parser_state);
static bool _parser_check_early_array_syntax(tjlang_parser_state* parser_state,u64* array_size);
static bool _parser_acquire_array(tjlang_parser_state* parser_state,tjlang_ir_instruction* new_array_instruction); 
static bool _parser_acquire_int_array(tjlang_parser_state* parser_state,i32** int_array,u64* array_size);
static bool _parser_acquire_uint_array(tjlang_parser_state* parser_state,u32** uint_array,u64* array_size);
static bool _parser_acquire_string_array(tjlang_parser_state* parser_state,string** str_array,u64* array_size);
void _parser_shutdown(void* token_data,void* parser_process_state);

#endif
