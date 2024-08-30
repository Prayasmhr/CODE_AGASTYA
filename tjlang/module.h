#ifndef _TJLANG_MODULE_H
#define _TJLANG_MODULE_H

#include"heart_lib/lib.h"
#include"central_enum_type.h"
#include"token_data_list.h"
#include"phase.h"
#include<stdio.h>
#include<stdlib.h>

typedef struct tjlang_data_flow {
    void* data_handle;
    void* process_state;
    void* prev_data_handle;
    phase_chain_node* phase;
} tjlang_data_flow;

typedef struct tjlang_central_state {
    tjlang_central_enum phase;
    tjlang_data_flow flow;
    tjlang_phase_interface _phase;
    char app_module[512];
} tjlang_central_state;

extern tjlang_central_state central_state;

inline static void set_app_module( char _app_module[512]) {snprintf(central_state.app_module,512,"%s",_app_module); }
inline static char* get_app_module(void) { return central_state.app_module; }
//void switch_to_phase(tjlang_central_enum phase);

#endif
