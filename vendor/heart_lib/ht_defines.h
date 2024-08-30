#ifndef _HT_DEFINES_H
#define _HT_DEFINES_H

#include<stdbool.h>

typedef short i16;
typedef int i32;
typedef long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef float f32;
typedef double f64;

typedef void* gcptr;
typedef const void* const_gcptr;

#define optional(type, var) struct {bool valid;type var;}

#endif
