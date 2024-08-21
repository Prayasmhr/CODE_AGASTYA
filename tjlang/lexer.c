#include"lexer.h"
#include"string.h"
#include"ctype.h"

#define KEYWORDS "__begin | __end | __module | __env_begin | __env_end"
#define TYPES " Int | UInt | String | Image | Font"

// helper structs for better code readibility 
struct _lexer_code_state {
    char* buffer;
    u32 size;
    u64 point;
};

struct _lexer_line_state {
    u64 n;
    u64 point; 
};

struct _lexer_analyze_state {
    char buffer[128];
    char c;
};

struct _lexer_core_state {
    FILE* write_file;
    linked_list* token_linkedlist;
};

struct tjlang_lexer_state {
    struct _lexer_code_state code;
    struct _lexer_line_state line;
    struct _lexer_analyze_state analyze;
    struct _lexer_core_state core;
};

void _lexer_init(const void* write_file,void** lexer_process_state){
    *lexer_process_state = malloc(sizeof(tjlang_lexer_state));
    tjlang_lexer_state* state = (tjlang_lexer_state*)(*lexer_process_state);
    state->line = (struct _lexer_line_state) {.n = 0,.point = 0};

    snprintf(state->analyze.buffer, 128, "");
    char app_module[64];
    char write_file_c[64];
    snprintf(app_module,64,"%s",(char*)write_file);
    snprintf(write_file_c,64,"./%s.c",(char*)write_file);

    state->core.write_file = fopen(write_file_c, "w"); //opens the file on write mod
    fclose(state->core.write_file); //clears the file content
    state->core.write_file = fopen(write_file_c, "a"); //opens the file again on append mode to append function code
    printf("%s\n",get_app_module());
    fprintf(state->core.write_file,"#include\"%s/module.h\"\n\n",app_module);

    state->core.token_linkedlist = tjlang_token_data_init();
}

static bool is_num(char* str){
    u64 size = strnlen(str, 128);
    for(int i = 0;i < size; i++){
        if(!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

static bool is_keyword(char* str) {
    return strstr(KEYWORDS,str) != NULL;
}

static bool is_type(char* str) {
    return strstr(TYPES, str) != NULL;
}

static bool read_file(char* path,char** out,u64* size) {
    FILE* f = fopen(path, "rb");
    if(!f){
        printf("failed to open file\n");
        return 0;
    }
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);
    *out = malloc(*size);
    fread(*out,1,*size,f);
    if(*out == NULL) {
        printf("failed to read file\n");
        return 0;
    }
    fclose(f);
    
    return 1;
}

void* _lexer_drive(const void* _file, void* lexer_process_state){
    u64 size = 0;
    tjlang_lexer_state* lexer_state = (tjlang_lexer_state*)lexer_process_state;
    read_file((char*)_file,&lexer_state->code.buffer,&size);

    lexer_state->code.point = 0;
    lexer_state->line.point = 0;
    lexer_state->code.size = size;
    lexer_state->analyze.c = lexer_state->code.buffer[lexer_state->code.point];

    while(lexer_state->analyze.c != EOF) {
        if(!_lexer_drive_internal(lexer_state)){
            break;
        }
    }

    _lexer_advance_with_identifier(lexer_state);
    lexer_state->code.point = 0;
    lexer_state->line.point = 0;
    lexer_state->line.n = 0;
    fclose(lexer_state->core.write_file);
    tjlang_token_data_info(lexer_state->core.token_linkedlist);

    return lexer_state->core.token_linkedlist;
}

static bool _lexer_drive_internal(tjlang_lexer_state* lexer_state) {
    switch(lexer_state->analyze.c) {
        case '\r':
            break;
        case ' ':
            _lexer_advance_with_identifier(lexer_state);
            break;
        case '\n':
            _lexer_advance_with_identifier(lexer_state);  
            lexer_state->line.n += 1;
            lexer_state->line.point = 0;
            break;
        case ':':
            _lexer_advance_with_colon(lexer_state);
            break;
        case '(':
            _lexer_advance_with_schar(lexer_state,TOKEN_TYPE_ROUND_LPAREN, 1);
            break;
        case '[':
            _lexer_advance_with_schar(lexer_state,TOKEN_TYPE_SQUARE_LPAREN, 1);
            break;
        case '{':
            _lexer_advance_with_schar(lexer_state,TOKEN_TYPE_CURLY_LPAREN, 1);
            break;
        case ')':
            _lexer_advance_with_schar(lexer_state,TOKEN_TYPE_ROUND_RPAREN, 1);
            break;
        case ']':
            _lexer_advance_with_schar(lexer_state,TOKEN_TYPE_SQUARE_RPAREN, 1);
            break;
        case '}':
            _lexer_advance_with_schar(lexer_state,TOKEN_TYPE_CURLY_RPAREN, 1);
            break;
        case '|':
            _lexer_advance_with_uientt_typespec(lexer_state);
            break;
        case '*':
            _lexer_advance_with_schar(lexer_state,TOKEN_TYPE_ASTERISK, 1);
            break;
        case '-': 
            _lexer_advance_with_operator(lexer_state);
            break;
        case '!':
            _lexer_advance_with_func(lexer_state);
            break;
        case '"':
            _lexer_advance_with_string(lexer_state);
            break;
        case ',':
            _lexer_advance_with_schar(lexer_state,TOKEN_TYPE_COMMA, 1);
            break; 
        default:
            char temp_string[2];
            snprintf(temp_string,2,"%c",lexer_state->analyze.c); 
            strncat_s(lexer_state->analyze.buffer,sizeof(lexer_state->analyze.buffer), temp_string,1);
            break;
        }
    if(lexer_state->code.point >= lexer_state->code.size) {
        return 0;
    }
    lexer_state->code.point++;
    lexer_state->line.point++;
    lexer_state->analyze.c = lexer_state->code.point >= lexer_state->code.size ? EOF : lexer_state->code.buffer[lexer_state->code.point];
    return 1;
}

void _lexer_advance_with_schar(tjlang_lexer_state* lexer_state,tjlang_central_enum type, bool with_identifier){
    if(with_identifier)
        _lexer_advance_with_identifier(lexer_state);
    char tmp_string[2];
    snprintf(tmp_string,2,"%c",lexer_state->analyze.c);
    tjlang_token_data_push(
        lexer_state->core.token_linkedlist,
        tmp_string,
        lexer_state->line.n,
        lexer_state->line.point,
        type
    );
}

void _lexer_advance_with_identifier(tjlang_lexer_state* lexer_state){
    if(strncmp(lexer_state->analyze.buffer,"",1) == 0) {
        return;
    }
    tjlang_central_enum token_type = TOKEN_TYPE_IDENTIFIER;
    if(is_keyword(lexer_state->analyze.buffer)) {
        token_type = TOKEN_TYPE_KEYWORD;
    }else if(is_num(lexer_state->analyze.buffer)) {
        token_type = TOKEN_TYPE_INTEGER;
    }else if(is_type(lexer_state->analyze.buffer)){
        token_type = TOKEN_TYPE_TYPE_SPEC;
    }
    tjlang_token_data_push(
        lexer_state->core.token_linkedlist,
        lexer_state->analyze.buffer,
        lexer_state->line.n,
        lexer_state->line.point,
        token_type
    );
    memset(lexer_state->analyze.buffer, 0, 128);
}

void _lexer_advance_with_operator(tjlang_lexer_state* lexer_state) {
    if(lexer_state->analyze.c == '-' && lexer_state->code.buffer[lexer_state->code.point+1] == '>') {
        tjlang_token_data_push(lexer_state->core.token_linkedlist,"->",lexer_state->line.n,lexer_state->line.point,TOKEN_TYPE_RIGHT_ARROW);
        lexer_state->code.point++;
    }
}

void _lexer_advance_with_colon(tjlang_lexer_state* lexer_state){
    _lexer_advance_with_identifier(lexer_state);
    
    if(lexer_state->code.buffer[lexer_state->code.point+1] == ':') {
        tjlang_token_data_push(lexer_state->core.token_linkedlist,"::",lexer_state->line.n,lexer_state->line.point,TOKEN_TYPE_DOUBLE_COLON);
        lexer_state->code.point++;
        return;
    }
    tjlang_token_data_push(lexer_state->core.token_linkedlist,":",lexer_state->line.n,lexer_state->line.point,TOKEN_TYPE_COLON);
}

void _lexer_advance_with_string(tjlang_lexer_state* lexer_state){
    char analyzed_buffer[512];
    snprintf(analyzed_buffer, 512, "");
    char q = lexer_state->analyze.c;
    u32 size = 0;
    
    size++;
    lexer_state->code.point++;
    lexer_state->analyze.c = lexer_state->code.buffer[lexer_state->code.point];
    char temp_string[2] = "";
    while(q != lexer_state->analyze.c && q != EOF){
        temp_string[0] = lexer_state->analyze.c;
        strncat_s(analyzed_buffer,sizeof(analyzed_buffer),temp_string,1);
        size++;
        lexer_state->code.point++;
        lexer_state->analyze.c = lexer_state->code.buffer[lexer_state->code.point];
    }   
    lexer_state->line.point += size - 1;
    
    tjlang_token_data_push(lexer_state->core.token_linkedlist,analyzed_buffer,lexer_state->line.n,lexer_state->line.point,TOKEN_TYPE_STRING);
}

void _lexer_advance_with_uientt_typespec(tjlang_lexer_state* lexer_state) {
    char uientt_spec_buffer[64] = "";
    u32 size = 0;

    lexer_state->code.point++;
    lexer_state->analyze.c = lexer_state->code.buffer[lexer_state->code.point];

    while (lexer_state->analyze.c != '|'){
        strncat_s(uientt_spec_buffer,sizeof(uientt_spec_buffer), &lexer_state->analyze.c,1);
        lexer_state->code.point++;
        lexer_state->analyze.c = lexer_state->code.buffer[lexer_state->code.point];
        size++;
    }
    lexer_state->line.point += size - 1;
    tjlang_token_data_push(lexer_state->core.token_linkedlist,uientt_spec_buffer,lexer_state->line.n,lexer_state->line.point,TOKEN_TYPE_UIENTT_SPEC);
}

void _lexer_advance_with_func(tjlang_lexer_state* lexer_state){
    char func_name[512] = "";
    lexer_state->code.point++;
    lexer_state->analyze.c = lexer_state->code.buffer[lexer_state->code.point];

    while (lexer_state->analyze.c != '{'){
        printf("%c",lexer_state->analyze.c);
        strncat_s(func_name,sizeof(func_name), &lexer_state->analyze.c,1);
        lexer_state->code.point++;
        lexer_state->analyze.c = lexer_state->code.buffer[lexer_state->code.point];
        lexer_state->line.point += 1;
        if(lexer_state->analyze.c == '\n') lexer_state->line.n += 1;
    }
    printf("function name : %s\n",func_name);
    fprintf(lexer_state->core.write_file, "_EXPORT void %s(tj_appstate* app_state, gcptr ctx_handle) ",func_name);
    while (lexer_state->analyze.c != '}'){
        fprintf(lexer_state->core.write_file, "%c", lexer_state->analyze.c);
        lexer_state->code.point++;
        lexer_state->analyze.c = lexer_state->code.buffer[lexer_state->code.point];
        lexer_state->line.point += 1;
    }

    fprintf(lexer_state->core.write_file, "}\n\n"); 
}

void _lexer_cleanup(void* file_path,void* lexer_process_state) {
    free(file_path);
}

void _lexer_close(void* data_handle,void* lexer_process_state){
    tjlang_lexer_state* lexer_state = (tjlang_lexer_state*)lexer_process_state;
    tjlang_token_data_free((linked_list*)data_handle);
    free(lexer_state->code.buffer);
    free(lexer_state);
}
