#ifndef _TJLANG_INTERPRETER_H
#define _TJLANG_INTERPRETER_H

#include"instruction_representation.h"
#include"../teja_ui/tj_renderer.h"
#include"../teja_ui/tj_ui_entity.h"

typedef struct tjlang_interpret_state tjlang_interpret_state;

void _interpreter_init(const void* ir,void** interpret_process_state);
void* _interpreter_interpret_ir(const void* ir,void* interpret_process_state);
static void _interpreter_interpret_zone(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone);
static void _interpreter_interpret_uienv_instruction(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction env_instruction); 
static void _interpreter_interpret_uientity_def(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction uientt_def_instruction); 
static void _interpreter_load_image_on_asset_tree(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction load_image_instruction); 
static void _interpreter_load_font_on_asset_tree(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction load_font_instruction);
static bool _interpreter_interpret_geometry(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction uientt_def_instruction,tj_uientity_ginfo* uientity);
static bool _interpreter_interpret_button(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction uientt_def_instruction,tj_uientity** button);
static bool _interpreter_interpret_label(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction uientt_def_instruction,tj_uientity** label);
void _interpreter_shutdown(void* ir_data,void* interpret_process_state);

#endif
