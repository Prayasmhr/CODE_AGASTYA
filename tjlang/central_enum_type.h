#ifndef _TJLANG_CENTRAL_ENUM_TYPE_H
#define _TJLANG_CENTRAL_ENUM_TYPE_H

#include"../heart_lib/ht_defines.h"

typedef enum tjlang_central_enum {
    // basic script types
    BASIC_LANG_TYPE_INTEGER,
    BASIC_LANG_TYPE_UINTEGER,
    BASIC_LANG_TYPE_STRING,

    // data types
    UIENTT_PROP_DATA_TYPE_NUMBER,
    UIENTT_PROP_DATA_TYPE_STRING,
    UIENTT_PROP_DATA_TYPE_ARRAY,
    UIENTT_PROP_DATA_TYPE_IDENTIFIER,

    //token types
    TOKEN_TYPE_UNKOWN,
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_UIENTT_SPEC,
    TOKEN_TYPE_TYPE_SPEC,
    TOKEN_TYPE_ROUND_LPAREN,
    TOKEN_TYPE_SQUARE_LPAREN,
    TOKEN_TYPE_CURLY_LPAREN,
    TOKEN_TYPE_ROUND_RPAREN,
    TOKEN_TYPE_SQUARE_RPAREN,
    TOKEN_TYPE_CURLY_RPAREN,
    TOKEN_TYPE_INTEGER,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_COMMA,
    TOKEN_TYPE_COLON,
    TOKEN_TYPE_SEMICOLON,
    TOKEN_TYPE_DOUBLE_COLON,
    TOKEN_TYPE_RIGHT_ARROW,
    TOKEN_TYPE_ASTERISK,

    // IR instruction types
    IR_ENV_DEFINTION,
    IR_UIENTT_DEFINITION,
    IR_SET_UIENTT_PROPERTY,
    IR_LOAD_ASSET,
    IR_SET_ASSET_SPEC,
    IR_NEW_ARRAY,

    //ENV and UIENTT instruction types
    ENV_BEGIN,
    ENV_END,
    UIENTT_BEGIN,
    UIENTT_END,

    //load asset instruction types
    LOAD_ASSET_FONT,
    LOAD_ASSET_IMAGE,

    //PHASE state
    PHASE_STATE_EARLY,
    PHASE_STATE_LEXER,
    PHASE_STATE_PARSER,
    PHASE_STATE_INTERPRETER
 
} tjlang_central_enum;


#endif
