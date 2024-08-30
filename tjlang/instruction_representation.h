#ifndef _TJLANG_INSTRUCTION_REPRESENTATION_H
#define _TJLANG_INSTRUCTION_REPRESENTATION_H

#include"module.h"
#include"../teja_ui/tj_asset.h"

typedef struct env_definition_instruction {
    string tag;
    u16 entities_defined;
    tjlang_central_enum type;
} env_definition_instruction;

typedef struct uientt_definition_instruction {
    string name;
    string uientt_type;
    tjlang_central_enum type;
} uientt_definition_instruction;

typedef struct set_uientt_prop_instruction {
    string prop;
    tjlang_central_enum data_type;
    gcptr value;
} set_uientt_prop_instruction;

typedef struct load_asset_instruction {
    tjlang_central_enum load_asset_type;
    char* path;
    string tag;
} load_asset_instruction;

typedef struct set_asset_spec_instruction {
    string spec;
    union {
        struct {
            tj_vec2i dimensions;
        } image;
        struct {
            u8 font_size;
        } font;
    } specs; 
} set_asset_spec_instruction;

typedef struct new_array_instruction {
    tjlang_central_enum element_type;
    union {
        string* str;
        i32* int32;
        u32* uint32;
    } arr; 
    u64 size;
} new_array_instruction;

typedef struct tjlang_ir_instruction {
    tjlang_central_enum type;
    union {
        env_definition_instruction env_definition;
        uientt_definition_instruction uientt_definition;
        set_uientt_prop_instruction set_uientt_prop;
        load_asset_instruction load_asset;
        set_asset_spec_instruction set_asset_spec;
        new_array_instruction new_array;  
    } data;
} tjlang_ir_instruction;

typedef struct tjlang_ir_zone {
    tjlang_ir_instruction* instructions;
    u8 size;
    u8 capacity;
} tjlang_ir_zone;

typedef struct tjlang_ir {
    linked_list* zone_linkedlist;
} tjlang_ir;

tjlang_ir* _ir_init(void);
tjlang_ir_zone* _ir_create_zone(u8 capacity);
void _ir_free_zone(void* zone);
void _ir_push_zone(tjlang_ir* ir,tjlang_ir_zone* chunk);
void _ir_zone_push_instruction(tjlang_ir_zone* chunk, tjlang_ir_instruction instruction);
void _ir_clear(tjlang_ir* ir);
void _ir_free(tjlang_ir* ir);

#endif
