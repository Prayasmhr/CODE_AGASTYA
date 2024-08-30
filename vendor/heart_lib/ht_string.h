#ifndef _HT_STRING_H
#define _HT_STRING_H

#include"ht_defines.h"

typedef struct string{
    char* data;
    u16 size;
} string;

string new_string(char* data);
string new_nstring(char* data, const u16 size);
bool string_nequal(const string lhs,const string rhs,const u16 max);
bool string_rnequal(const string lhs,const char* rhs,const u16 max);
i32 string_compare(const string lhs,const string rhs);
string string_concatenate(const string lhs,const char* str,const u16 max);
string string_cconcatenate(const string lhs,const string rhs,const u16 max);
bool string_char(const string self, const char C,char* out);
void string_trim(string* self);
void string_replace(string* self, char target,char replace);
void string_free(string* self);

#endif
