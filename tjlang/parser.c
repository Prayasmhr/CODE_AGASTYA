#include"parser.h"

#define _PARSER_NEXT_TOKEN(_state) (tjlang_token*)(_state->iter_node->next->data)
#define _PARSER_ADVANCE_TOKEN(_state) \
    _state->iter_node = _state->iter_node->next;\
    if(_state->iter_node != NULL) \
        _state->curr_token = (tjlang_token*)_state->iter_node->data\

struct _parser_uienv_process_state {
    bool inside_uienv;
    u16 count;
};

struct _parser_core_state {
    bool _error;
    tjlang_ir* ir;
};

typedef struct tjlang_parser_state {
    tjlang_token* curr_token;
    ll_node* iter_node;
    struct _parser_uienv_process_state uienv_process;
    struct _parser_core_state core;
} tjlang_parser_state;

#define PARSER_LOG(msg,line,code_point) printf("TJSCRIPT ERR: %s", msg); printf("AT LINE: %d CODEPOINT: %d\n",line,code_point);

void _parser_init(const void* token_data,void** parser_process_state) {
    *parser_process_state = malloc(sizeof(tjlang_parser_state));
    tjlang_parser_state* parser_state = (tjlang_parser_state*)(*parser_process_state);
    parser_state->core.ir = _ir_init(); 
    parser_state->core._error = 0;
    parser_state->uienv_process.inside_uienv = 0;
    parser_state->uienv_process.count = 0;
}

void* _parser_process(const void* token_data,void* parser_process_state) {
    tjlang_parser_state* parser_state = (tjlang_parser_state*)parser_process_state;
    linked_list* token_data_list = (linked_list*)token_data;
    parser_state->iter_node = token_data_list->head;

    while (parser_state->iter_node != NULL) {
        _parser_process_internal(parser_state);
    }

    return parser_state->core.ir;
}

static void _parser_process_internal(tjlang_parser_state* parser_state) {
    parser_state->curr_token = (tjlang_token*)parser_state->iter_node->data;
    if(parser_state->curr_token->type == TOKEN_TYPE_DOUBLE_COLON) {
        _parser_handle_asset_def(parser_state);
    }else if(parser_state->curr_token->type == TOKEN_TYPE_KEYWORD) {
        _parser_handle_keyword(parser_state);
    }
}

static void _parser_handle_keyword(tjlang_parser_state* parser_state) {
    if(strncmp(parser_state->curr_token->data.data, "__env_begin", parser_state->curr_token->data.size) == 0) {
        _PARSER_ADVANCE_TOKEN(parser_state);
        if(parser_state->curr_token->type != TOKEN_TYPE_IDENTIFIER) {
            PARSER_LOG("expected a identifier | env name",parser_state->curr_token->line,parser_state->curr_token->line_point);
            parser_state->core._error = 1;
        }
        tjlang_ir_zone* env_begin_zone = _ir_create_zone(1);
        tjlang_ir_instruction env_begin_instruction = (tjlang_ir_instruction) {
            .type = IR_ENV_DEFINTION,
            .data.env_definition.tag = new_nstring(parser_state->curr_token->data.data,parser_state->curr_token->data.size),
            .data.env_definition.type = ENV_BEGIN
        };

        _ir_zone_push_instruction(env_begin_zone, env_begin_instruction);
        _ir_push_zone(parser_state->core.ir, env_begin_zone);
        _PARSER_ADVANCE_TOKEN(parser_state);
        parser_state->uienv_process.inside_uienv = 1;
    }else if(strncmp(parser_state->curr_token->data.data, "__env_end", parser_state->curr_token->data.size) == 0) {
        tjlang_ir_zone* env_end_zone = _ir_create_zone(1);
        tjlang_ir_instruction env_end_instruction = (tjlang_ir_instruction) {
            .type = IR_ENV_DEFINTION,
            .data.env_definition.type = ENV_END,
            .data.env_definition.entities_defined = parser_state->uienv_process.count
        };
        parser_state->uienv_process.count = 0;
        parser_state->uienv_process.inside_uienv = 0;
        _ir_zone_push_instruction(env_end_zone, env_end_instruction);
        _ir_push_zone(parser_state->core.ir, env_end_zone);
        _PARSER_ADVANCE_TOKEN(parser_state);
    }else if(strncmp(parser_state->curr_token->data.data, "__begin", parser_state->curr_token->data.size) == 0) {
        if(!parser_state->uienv_process.inside_uienv) {
            parser_state->core._error = 1;
            PARSER_LOG("ui entity not inside any ui environment",parser_state->curr_token->line,parser_state->curr_token->line_point);
        }
        tjlang_ir_zone* uientt_def_zone = _ir_create_zone(16);
        tjlang_token* name_token = _PARSER_NEXT_TOKEN(parser_state); 
        if(name_token->type != TOKEN_TYPE_IDENTIFIER) {
            PARSER_LOG("expected a identifier | ui_entity name",name_token->line,name_token->line_point);
            parser_state->core._error = 1;
        }
        _PARSER_ADVANCE_TOKEN(parser_state);

        tjlang_token* uientt_typespec_token = _PARSER_NEXT_TOKEN(parser_state); 
        if(uientt_typespec_token->type != TOKEN_TYPE_UIENTT_SPEC) {
            PARSER_LOG("expected |uientt_typespec|",uientt_typespec_token->line,uientt_typespec_token->line_point);
            parser_state->core._error = 1;
        } 
        
        tjlang_ir_instruction uientt_defbegin_instruction = (tjlang_ir_instruction) {
            .type = IR_UIENTT_DEFINITION,
            .data.uientt_definition.name = new_nstring(name_token->data.data,name_token->data.size),
            .data.uientt_definition.uientt_type = new_nstring(uientt_typespec_token->data.data, uientt_typespec_token->data.size),
            .data.uientt_definition.type = UIENTT_BEGIN
        };

        _ir_zone_push_instruction(uientt_def_zone, uientt_defbegin_instruction);
        _PARSER_ADVANCE_TOKEN(parser_state);

        printf("%s",parser_state->curr_token->data.data);
        _parser_acquire_uientt_prop_assignments(parser_state,uientt_def_zone);
        _ir_push_zone(parser_state->core.ir, uientt_def_zone);
        parser_state->uienv_process.count += 1;
        _PARSER_ADVANCE_TOKEN(parser_state);
    }
}

static tjlang_ir_instruction _parser_handle_uientt_prop_assign(tjlang_parser_state* parser_state) {
    if(parser_state->curr_token->type != TOKEN_TYPE_IDENTIFIER) {
        PARSER_LOG("expected property assignment",parser_state->curr_token->line,parser_state->curr_token->line_point);
        parser_state->core._error = 1;
    }
    char* prop = parser_state->curr_token->data.data;

    _PARSER_ADVANCE_TOKEN(parser_state);
    if(parser_state->curr_token->type != TOKEN_TYPE_COLON) {
        PARSER_LOG("expected a colon after property name\n",parser_state->curr_token->line,parser_state->curr_token->line_point);
    }
    _PARSER_ADVANCE_TOKEN(parser_state);
    tjlang_ir_instruction prop_assignment_instruction;
    prop_assignment_instruction.type = IR_SET_UIENTT_PROPERTY;
    prop_assignment_instruction.data.set_uientt_prop.prop = new_string(prop);

    if(parser_state->curr_token->type == TOKEN_TYPE_SQUARE_LPAREN) {
        prop_assignment_instruction.data.set_uientt_prop.data_type = UIENTT_PROP_DATA_TYPE_ARRAY;
        
        tjlang_ir_instruction new_array_instruction;
        new_array_instruction.type = IR_NEW_ARRAY;
        if(!_parser_acquire_array(parser_state,&new_array_instruction)) {
            parser_state->core._error = 1;
            return new_array_instruction;
        }
        prop_assignment_instruction.data.set_uientt_prop.value = malloc(sizeof(tjlang_ir_instruction));
        memcpy(
            prop_assignment_instruction.data.set_uientt_prop.value,
            (gcptr)&new_array_instruction,
            sizeof(new_array_instruction));
        memset(&new_array_instruction, 0, sizeof(tjlang_ir_instruction));
    }else if(parser_state->curr_token->type == TOKEN_TYPE_INTEGER) {
        prop_assignment_instruction.data.set_uientt_prop.data_type = UIENTT_PROP_DATA_TYPE_NUMBER;
        prop_assignment_instruction.data.set_uientt_prop.value = (void*)(intptr_t)strtol(parser_state->curr_token->data.data, &parser_state->curr_token->data.data, 0);
    }else if(parser_state->curr_token->type == TOKEN_TYPE_STRING) {
        prop_assignment_instruction.data.set_uientt_prop.data_type = UIENTT_PROP_DATA_TYPE_STRING;
        prop_assignment_instruction.data.set_uientt_prop.value = malloc(sizeof(char) * (parser_state->curr_token->data.size + 1));
        snprintf((char*)prop_assignment_instruction.data.set_uientt_prop.value,parser_state->curr_token->data.size+1,"%s",parser_state->curr_token->data.data);
    }else if(parser_state->curr_token->type == TOKEN_TYPE_IDENTIFIER) {
        prop_assignment_instruction.data.set_uientt_prop.data_type = UIENTT_PROP_DATA_TYPE_IDENTIFIER;
        prop_assignment_instruction.data.set_uientt_prop.value = malloc(sizeof(char) * (parser_state->curr_token->data.size + 1));
        snprintf((char*)prop_assignment_instruction.data.set_uientt_prop.value,parser_state->curr_token->data.size+1,"%s",parser_state->curr_token->data.data);
    }

    return prop_assignment_instruction;    
}

static void _parser_acquire_uientt_prop_assignments(tjlang_parser_state* parser_state,tjlang_ir_zone* uientt_def_zone) {
    _PARSER_ADVANCE_TOKEN(parser_state);
    tjlang_token* next_token = _PARSER_NEXT_TOKEN(parser_state);
    if(next_token->type == TOKEN_TYPE_KEYWORD && strncmp(parser_state->curr_token->data.data, "__end", parser_state->curr_token->data.size) == 0 ) {
        PARSER_LOG("expected properties",next_token->line,next_token->line_point);
        parser_state->core._error = 1;
        return;
    }
    
    while (next_token->type != TOKEN_TYPE_KEYWORD && strncmp(parser_state->curr_token->data.data, "__end", parser_state->curr_token->data.size) != 0) {
        tjlang_ir_instruction prop_assignment_instruction = _parser_handle_uientt_prop_assign(parser_state);
        _ir_zone_push_instruction(uientt_def_zone, prop_assignment_instruction);
        _PARSER_ADVANCE_TOKEN(parser_state);
    }

    tjlang_ir_instruction uientt_def_end_instruction = (tjlang_ir_instruction) {
        .type = IR_UIENTT_DEFINITION,
        .data.uientt_definition.type = UIENTT_END
    };

    _ir_zone_push_instruction(uientt_def_zone, uientt_def_end_instruction);

}

static void _parser_handle_asset_def(tjlang_parser_state* parser_state) {
    _PARSER_ADVANCE_TOKEN(parser_state);
    tjlang_ir_zone* asset_load_zone = _ir_create_zone(4);
    tjlang_ir_instruction asset_load_instruction;
    asset_load_instruction.type = IR_LOAD_ASSET;
    
    if(parser_state->curr_token->type != TOKEN_TYPE_IDENTIFIER) {
        PARSER_LOG("expected asset name",parser_state->curr_token->line,parser_state->curr_token->line_point);
        parser_state->core._error = 1;
        return;
    }
    asset_load_instruction.data.load_asset.tag = new_nstring(parser_state->curr_token->data.data,parser_state->curr_token->data.size);
    _PARSER_ADVANCE_TOKEN(parser_state); 
    if(parser_state->curr_token->type != TOKEN_TYPE_TYPE_SPEC) {
        PARSER_LOG("expected asset type [ Image | Font]",parser_state->curr_token->line,parser_state->curr_token->line_point);
        parser_state->core._error = 1;
        return;
    }
    tjlang_token* asset_type_token = (tjlang_token*)parser_state->curr_token;

    _PARSER_ADVANCE_TOKEN(parser_state);
    if(parser_state->curr_token->type != TOKEN_TYPE_STRING) {
        PARSER_LOG("expected asset path | String",parser_state->curr_token->line,parser_state->curr_token->line_point);
        parser_state->core._error = 1;
        return;
    }

    if(strncmp(asset_type_token->data.data,"Font",5) == 0) {
        asset_load_instruction.data.load_asset.load_asset_type = LOAD_ASSET_FONT;
        asset_load_instruction.data.load_asset.path = malloc(parser_state->curr_token->data.size + 1);
        snprintf(asset_load_instruction.data.load_asset.path, parser_state->curr_token->data.size + 1,"%s",parser_state->curr_token->data.data);
        printf("FONT PATH: %s\n",asset_load_instruction.data.load_asset.path);
        _ir_zone_push_instruction(asset_load_zone,asset_load_instruction);
        _parser_acquire_font_specs(parser_state,asset_load_zone);
    }else if(strncmp(asset_type_token->data.data,"Image",6) == 0) {
        asset_load_instruction.data.load_asset.load_asset_type = LOAD_ASSET_IMAGE;
        asset_load_instruction.data.load_asset.path = malloc(parser_state->curr_token->data.size + 1);
        snprintf(asset_load_instruction.data.load_asset.path, parser_state->curr_token->data.size + 1, "%s",parser_state->curr_token->data.data);
        printf("%s\n",asset_load_instruction.data.load_asset.path);
        _ir_zone_push_instruction(asset_load_zone,asset_load_instruction);
        _parser_acquire_image_specs(parser_state,asset_load_zone);
    }
    _ir_push_zone(parser_state->core.ir, asset_load_zone);
    _PARSER_ADVANCE_TOKEN(parser_state);
}

static bool _parser_acquire_array(tjlang_parser_state* parser_state,tjlang_ir_instruction* new_array_instruction) {
    tjlang_token* typespec_token = _PARSER_NEXT_TOKEN(parser_state); 
    if(typespec_token->type != TOKEN_TYPE_TYPE_SPEC) {
        PARSER_LOG("expected type name | only primitive types allowed",typespec_token->line,typespec_token->line_point);
        return 0;
    }
    _PARSER_ADVANCE_TOKEN(parser_state);

    if(strncmp(typespec_token->data.data, "Int", 4) == 0) {
        i32* int_array;
        new_array_instruction->data.new_array.element_type = BASIC_LANG_TYPE_INTEGER;
        if(!_parser_acquire_int_array(parser_state,&int_array, &new_array_instruction->data.new_array.size)){
            return 0;
        }

        new_array_instruction->data.new_array.arr.int32 = int_array;
    }else if(strncmp(typespec_token->data.data, "UInt", 5) == 0) {
        u32* uint_array;
        new_array_instruction->data.new_array.element_type = BASIC_LANG_TYPE_UINTEGER;
        if(!_parser_acquire_uint_array(parser_state,&uint_array, &new_array_instruction->data.new_array.size)) {
            return 0;
        }
        new_array_instruction->data.new_array.arr.uint32 = uint_array;
    }else if(strncmp(typespec_token->data.data, "String", 7) == 0) {
        string* str_array;
        new_array_instruction->data.new_array.element_type = BASIC_LANG_TYPE_STRING;
        if(!_parser_acquire_string_array(parser_state,&str_array, &new_array_instruction->data.new_array.size)) {
            return 0;
        }
        new_array_instruction->data.new_array.arr.str = str_array;
    }
    return 1;
}

static bool _parser_check_early_array_syntax(tjlang_parser_state* parser_state,u64* array_size) {
    _PARSER_ADVANCE_TOKEN(parser_state);
    if(parser_state->curr_token->type != TOKEN_TYPE_SQUARE_RPAREN) {
        PARSER_LOG("expected closing brackets after typespec",parser_state->curr_token->line,parser_state->curr_token->line_point);
        return 0;
    }
    _PARSER_ADVANCE_TOKEN(parser_state);

    if(parser_state->curr_token->type != TOKEN_TYPE_ASTERISK) {
        PARSER_LOG("expected TOKEN_TYPE_ASTERISK to specify array size",parser_state->curr_token->line, parser_state->curr_token->line_point);
        return 0;
    }
    _PARSER_ADVANCE_TOKEN(parser_state);;

    if(parser_state->curr_token->type != TOKEN_TYPE_INTEGER) {
        PARSER_LOG("expected array size integer",parser_state->curr_token->line,parser_state->curr_token->line_point);
        return 0;
    }

    *array_size = strtol(parser_state->curr_token->data.data, NULL, 10);
    if(*array_size <= 0) {
        PARSER_LOG("size must be a postive integer",parser_state->curr_token->line,parser_state->curr_token->line_point);
        return 0;
    }
    _PARSER_ADVANCE_TOKEN(parser_state);
    if(parser_state->curr_token->type != TOKEN_TYPE_RIGHT_ARROW) {
        PARSER_LOG("expecting -> to point to a array",parser_state->curr_token->line,parser_state->curr_token->line_point);
        return 0;
    }

    _PARSER_ADVANCE_TOKEN(parser_state);
    if(parser_state->curr_token->type != TOKEN_TYPE_CURLY_LPAREN) {
        PARSER_LOG("expecting a { for array", parser_state->curr_token->line,parser_state->curr_token->line_point);
        return 0;
    }

    return 1;
}

static bool _parser_acquire_int_array(tjlang_parser_state* parser_state,i32** int_array, u64* array_size) {
    if(!_parser_check_early_array_syntax(parser_state,array_size))
        return 0;
    
    tjlang_token* prev_token = parser_state->curr_token; 
    _PARSER_ADVANCE_TOKEN(parser_state);
    u64 capacity = 8;
    u64 size = 0;
    *int_array = malloc(sizeof(i32) * capacity);

    while (1) {
        if(parser_state->curr_token->type == TOKEN_TYPE_INTEGER && (prev_token->type == TOKEN_TYPE_COMMA || prev_token->type == TOKEN_TYPE_CURLY_LPAREN)) {
            (*int_array)[size] = strtol(parser_state->curr_token->data.data, NULL, 10);
            size++;
        }else if(prev_token->type == TOKEN_TYPE_INTEGER && parser_state->curr_token->type != TOKEN_TYPE_COMMA) {
            PARSER_LOG("expected TOKEN_TYPE_COMMA (,)",prev_token->line,prev_token->line_point);
            return 0;
        }else if(prev_token->type == TOKEN_TYPE_COMMA && parser_state->curr_token->type != TOKEN_TYPE_INTEGER) {
            PARSER_LOG("expected a integer after TOKEN_TYPE_COMMA",prev_token->line,prev_token->line_point);
            return 0;
        }

        prev_token = parser_state->curr_token;
        _PARSER_ADVANCE_TOKEN(parser_state);
        if(parser_state->curr_token->type == TOKEN_TYPE_CURLY_RPAREN) {
            return 1;
        }

        
        if(size + 1 > capacity) {
            capacity += 3;
            *int_array = realloc(*int_array, sizeof(i32) * capacity);
        }

        if(size > *array_size) {
            printf("array size exceeds the limits\n");
            return 0;  
        }
    }

    return 1;
}

static bool _parser_acquire_uint_array(tjlang_parser_state* parser_state,u32** uint_array, u64* array_size) {
    if(!_parser_check_early_array_syntax(parser_state,array_size))
        return 0;

    tjlang_token* prev_token = parser_state->curr_token; 
    _PARSER_ADVANCE_TOKEN(parser_state);
    u64 capacity = 8;
    u64 size = 0;
    *uint_array = malloc(sizeof(i32) * capacity);

    while (1) {
        if(parser_state->curr_token->type == TOKEN_TYPE_INTEGER && (prev_token->type == TOKEN_TYPE_COMMA || prev_token->type == TOKEN_TYPE_CURLY_LPAREN)) {
            if(string_char(parser_state->curr_token->data,'-',NULL)) {
                PARSER_LOG("expected a unsigned integer",parser_state->curr_token->line,parser_state->curr_token->line_point);
                return 0;
            }
            (*uint_array)[size] = strtoul(parser_state->curr_token->data.data, NULL, 10);
            size++;
        }else if(prev_token->type == TOKEN_TYPE_INTEGER && parser_state->curr_token->type != TOKEN_TYPE_COMMA) {
            PARSER_LOG("expected TOKEN_TYPE_COMMA (,)",prev_token->line,prev_token->line_point);
            return 0;
        }else if(prev_token->type == TOKEN_TYPE_COMMA && parser_state->curr_token->type != TOKEN_TYPE_INTEGER) {
            PARSER_LOG("expected a unsigned integer after TOKEN_TYPE_COMMA",prev_token->line,prev_token->line_point);
            return 0;
        }

        prev_token = parser_state->curr_token;
        _PARSER_ADVANCE_TOKEN(parser_state);
        if(parser_state->curr_token->type == TOKEN_TYPE_CURLY_RPAREN) {
            return 1;
        }
        
        if(size + 1 > capacity) {
            capacity += 3;
            *uint_array = realloc(*uint_array, sizeof(i32) * capacity);
        }
        if(size > *array_size) {
            printf("array size exceeds the limits\n");
            return 0;  
        }
    }

    return 1;
}

static bool _parser_acquire_string_array(tjlang_parser_state* parser_state,string** str_array, u64* array_size) {
    _parser_check_early_array_syntax(parser_state,array_size);

    tjlang_token* prev_token = parser_state->curr_token; 
    _PARSER_ADVANCE_TOKEN(parser_state);
    u64 capacity = 8;
    u64 size = 0;
    *str_array = malloc(sizeof(string) * capacity);

    while (1) {
        if(parser_state->curr_token->type == TOKEN_TYPE_STRING && (prev_token->type == TOKEN_TYPE_COMMA || prev_token->type == TOKEN_TYPE_CURLY_LPAREN)) {
            (*str_array)[size] = parser_state->curr_token->data;
            size++;
        }else if(prev_token->type == TOKEN_TYPE_STRING && parser_state->curr_token->type != TOKEN_TYPE_COMMA) {
            PARSER_LOG("expected TOKEN_TYPE_COMMA (,)",prev_token->line,prev_token->line_point);
            return 0;
        }else if(prev_token->type == TOKEN_TYPE_COMMA && parser_state->curr_token->type != TOKEN_TYPE_STRING) {
            PARSER_LOG("expected a string after TOKEN_TYPE_COMMA",prev_token->line,prev_token->line_point);
            return 0;
        }

        prev_token = parser_state->curr_token;
        _PARSER_ADVANCE_TOKEN(parser_state);
        if(parser_state->curr_token->type == TOKEN_TYPE_CURLY_RPAREN) {
            return 1;
        }
        
        if(size + 1 > capacity) {
            capacity += 3;
            *str_array = realloc(*str_array, sizeof(string) * capacity);
        }
        if(size > *array_size) {
            printf("array size exceeds the limits\n");
            return 0;  
        }
    };

    return 1;
}

static void _parser_acquire_image_specs(tjlang_parser_state* parser_state,tjlang_ir_zone* asset_load_zone) {
    tjlang_token* round_lparen_token = _PARSER_NEXT_TOKEN(parser_state); 
    bool custom_size = round_lparen_token->type == TOKEN_TYPE_ROUND_LPAREN;
    tj_vec2i dimension;
    _PARSER_ADVANCE_TOKEN(parser_state);
    i32* vec2 = malloc(sizeof(i32) * 2);

    if(custom_size){
        _PARSER_ADVANCE_TOKEN(parser_state);
        if(parser_state->curr_token->type != TOKEN_TYPE_SQUARE_LPAREN) {
            PARSER_LOG("expected a vec2 / array of 2 integers",parser_state->curr_token->line,parser_state->curr_token->line_point);
        }
        _PARSER_ADVANCE_TOKEN(parser_state);
        if(parser_state->curr_token->type != TOKEN_TYPE_TYPE_SPEC && strncmp(parser_state->curr_token->data.data,"UInt",3) != 0) {
            PARSER_LOG("expected a vec2 / array of 2 unsigned integers",parser_state->curr_token->line,parser_state->curr_token->line_point);
        }

        u64 size = 0;
        if(!_parser_acquire_int_array(parser_state,&vec2,&size)) {
            PARSER_LOG("error handling arrays\n",round_lparen_token->line,round_lparen_token->line_point);
            parser_state->core._error = 1;
            return;
        }
        if(size != 2) {
            PARSER_LOG("dimensions can only expect array of 2 integers\n",round_lparen_token->line,round_lparen_token->line_point);
            parser_state->core._error = 1;
            return;
        }

        dimension.x = vec2[0];
        dimension.y = vec2[1];
        free(vec2);
    }

    tjlang_ir_instruction image_spec_instruction = (tjlang_ir_instruction) {
        .type = IR_SET_ASSET_SPEC,
        .data.set_asset_spec.spec = custom_size ? new_string("c_dimension") : new_string("d_dimension"),
        .data.set_asset_spec.specs.image.dimensions = custom_size ? dimension : (tj_vec2i) {.x = 0,.y = 0}
    };
    _ir_zone_push_instruction(asset_load_zone, image_spec_instruction);
    
    tjlang_token* round_rparen_token = _PARSER_NEXT_TOKEN(parser_state);
    if(round_rparen_token->type != TOKEN_TYPE_ROUND_RPAREN) {
        PARSER_LOG("expected round parenthesis",round_lparen_token->line,round_lparen_token->line_point);
        parser_state->core._error = 1;
        return;        
    }
    _PARSER_ADVANCE_TOKEN(parser_state);
}

static void _parser_acquire_font_specs(tjlang_parser_state* parser_state,tjlang_ir_zone* asset_load_zone) {
    tjlang_token* round_lparen_token = _PARSER_NEXT_TOKEN(parser_state); 
    u8 final_size = 0;
    if(round_lparen_token->type != TOKEN_TYPE_ROUND_LPAREN) {
        final_size = 32;
    }
    _PARSER_ADVANCE_TOKEN(parser_state);;

    if(final_size == 0 ){
        tjlang_token* font_size_token = _PARSER_NEXT_TOKEN(parser_state); 
        if(font_size_token->type != TOKEN_TYPE_INTEGER) {
            PARSER_LOG("expected a integer for a font size",font_size_token->line,font_size_token->line_point);
            parser_state->core._error = 1;
            return;
        }
        final_size = strtol(font_size_token->data.data, NULL, 10);
    }

    tjlang_ir_instruction font_spec_instruction = (tjlang_ir_instruction) {
        .type = IR_SET_ASSET_SPEC,
        .data.set_asset_spec.spec = new_string("font_size"),
        .data.set_asset_spec.specs.font.font_size = final_size
    };

    _ir_zone_push_instruction(asset_load_zone, font_spec_instruction);
    _PARSER_ADVANCE_TOKEN(parser_state);;
    
    tjlang_token* round_rparen_token = _PARSER_NEXT_TOKEN(parser_state);
    if(round_rparen_token->type != TOKEN_TYPE_ROUND_RPAREN) {
        PARSER_LOG("expected round parenthesis",round_lparen_token->line,round_lparen_token->line_point);
        parser_state->core._error = 1;
        return;        
    }
    _PARSER_ADVANCE_TOKEN(parser_state);
}

void _parser_shutdown (void* ir,void* parser_process_data) {
    _ir_free(ir);
    free(parser_process_data);
}
