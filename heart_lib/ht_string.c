#include"ht_string.h"
#include"stdlib.h"
#include"stdio.h"
#include"string.h"

string new_string(char* data){
    string out;
    out.size = strnlen(data, 512); 
    out.data = malloc((out.size + 1) * sizeof(char));
    snprintf(out.data, out.size + 1, "%s", data);
    return out;
}

string new_nstring(char* data, const u16 size){
    string out;
    out.size = size;
    out.data = malloc((out.size + 1) * sizeof(char));
    snprintf(out.data, out.size + 1, "%s", data);
    return out;
}

bool string_nequal(const string lhs,const string rhs,const u16 max){
    if(lhs.size != rhs.size) {
        return 0;
    }else if(lhs.data[0] != rhs.data[0] || lhs.data[lhs.size - 1] != rhs.data[rhs.size - 1]){
        return 0;
    } 
    return strncmp(lhs.data, rhs.data, max < lhs.size ? max : lhs.size) == 0;
}

void string_cut(string *lhs, const u16 from){
    lhs->data = realloc(lhs->data, from - 1);
    lhs->size = from;
}

void string_free(string* self){
    if(self->data != NULL) free(self->data); self->data = NULL;
}

string string_concatenate(const string lhs,const char* str,const u16 max){
    u16 rhs_len = strnlen(str, 128);
    u16 final_max = rhs_len > max ? max : rhs_len; 

    string out = new_nstring("", lhs.size + final_max);
    snprintf(out.data, lhs.size + final_max + 1, "%s%s", lhs.data, str);
    
    return out;
}

string string_cconcatenate(const string lhs,const string rhs,const u16 max){
    u16 final_max = rhs.size > max ? max : rhs.size;
    string out = new_nstring("", lhs.size + max);
    snprintf(out.data, lhs.size + final_max + 1, "%s%s", lhs.data, rhs.data); 
    return out;
}

i32 string_compare(const string lhs,const string rhs){
    return strncmp(lhs.data,rhs.data,rhs.size > lhs.size ? lhs.size : rhs.size);
}

bool string_char(const string self, const char C,char* out){
    for(u16 i = 0;i < self.size; i++){
        if(self.data[i] == C) {
            if(out == NULL) {
                return 1;
            }
            *out = i;
            return 1;
        }
    }

    return 0;
}

/*
void string_trim(string* self){

}
*/
void string_replace(string* self, char target,char replace){
    for(int i = 0;i < self->size; i++){
        if(self->data[i] == target) {
            self->data[i] = replace;
        }
    }
}
