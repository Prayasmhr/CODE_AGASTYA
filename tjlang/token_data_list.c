#include"token_data_list.h"
#include"stdlib.h"

linked_list* tjlang_token_data_init(void) {
    return linkedlist_create(sizeof(tjlang_token));
}

void tjlang_token_data_push(linked_list* token_linkedlist,char* data,u32 line,u16 line_point,tjlang_central_enum type) {
    tjlang_token* token = malloc(sizeof(tjlang_token));
    token->data = new_string(data),
    token->line = line;
    token->line_point = line_point;
    token->type = type;
    linkedlist_insert(token_linkedlist,token);
}

static void tjlang_token_node_info(FILE* log_file,ll_node* _node) {
    if(_node == NULL) return;
    tjlang_token* _token = (tjlang_token*)_node->data;
    
    fprintf(log_file, "tjlang token type");
    fprintf(log_file," -> %d |",_token->type);
    fprintf(log_file," data -> %s |",_token->data.data);
    fprintf(log_file," line -> %d |",_token->line);
    fprintf(log_file," line_point -> %d \n",_token->line_point);

    tjlang_token_node_info(log_file,_node->next);
}

void tjlang_token_data_info(linked_list* token_linkedlist) {
    FILE* log_file = fopen("tjlang_log.txt","w");
    tjlang_token_node_info(log_file,token_linkedlist->head);
    fclose(log_file);
}

void token_free(void* token) {
    string_free(&((tjlang_token*)token)->data);   
    free(token);
}

void tjlang_token_data_free(linked_list* token_linkedlist) {
    linkedlist_destroy(token_linkedlist,token_free);
}
