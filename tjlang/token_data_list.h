#ifndef _TJLANG_TOKEN_DATA_LIST_H
#define _TJLANG_TOKEN_DATA_LIST_H

#include"central_enum_type.h"
#include"heart_lib/ht_string.h"
#include"heart_lib/ht_linkedlist.h"

typedef struct tjlang_token{
    string data;
    u32 line;
    u16 line_point;
    tjlang_central_enum type;
} tjlang_token;

linked_list* tjlang_token_data_init(void);
void tjlang_token_data_push(linked_list* token_linkedlist,char* data,u32 line,u16 line_point,tjlang_central_enum type);
void tjlang_token_data_info(linked_list* token_linkedlist);
void tjlang_token_data_free(linked_list* token_linkedlist);

#endif
