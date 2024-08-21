#include"tjlang.h"

void tjlang_interpret(char* app_module_name,char *file_name)
{
    set_app_module(app_module_name);
    central_state.phase = PHASE_STATE_EARLY;
    central_state.flow.process_state = NULL;
    central_state.flow.data_handle = (void*)file_name;
    central_state.flow.prev_data_handle = central_state.flow.data_handle;
    tjlang_lexer();
    central_state._phase._init(central_state.flow.data_handle,&central_state.flow.process_state);
    central_state.flow.data_handle = central_state._phase._process(central_state.flow.data_handle,central_state.flow.process_state);

    tjlang_parser();
    void* prev_process_state = central_state.flow.process_state;
    central_state._phase._init(central_state.app_module,&central_state.flow.process_state);
    central_state.flow.prev_data_handle = central_state.flow.data_handle;
    central_state.flow.data_handle = central_state._phase._process(central_state.flow.data_handle,central_state.flow.process_state);
    central_state._phase._cleanup(central_state.flow.prev_data_handle,prev_process_state);

    char command[512] = "";
    snprintf(command,512,"clang %s.c -shared -o %s.dll",app_module_name,app_module_name); //this is probably wrong,the wrong file is getting compiled but let me first fix that uienv problem (anoying litle ahh piece of sh!it)
    system(command);

    tjlang_interpreter();
    central_state._phase._init(central_state.flow.data_handle,&central_state.flow.process_state);
    central_state.flow.prev_data_handle = central_state.flow.data_handle;
    central_state.flow.data_handle = central_state._phase._process(central_state.flow.data_handle,central_state.flow.process_state);
}

static void tjlang_lexer(void) {
    central_state._phase._init = _lexer_init;
    central_state._phase._process = _lexer_drive;
}

static void tjlang_parser(void) {
    central_state._phase._init = _parser_init;
    central_state._phase._process = _parser_process;
    central_state._phase._cleanup = _lexer_close;
}

static void tjlang_interpreter(void) {
    central_state._phase._init = _interpreter_init;
    central_state._phase._process = _interpreter_interpret_ir;
}
