#ifndef _TJ_RENDERTYPES_H
#define _TJ_RENDERTYPES_H

#include"heart_lib/ht_defines.h"

typedef struct {
    f32 x;
    f32 y;
    f32 w;
    f32 h;
    u8 round_corner;
} tj_rectangle;

typedef struct {
    u8 R;
    u8 B;
    u8 G;
    u8 A;
} tj_color;

#define WHITE        (tj_color) {.R = 255,.G = 255,.B = 255,.A = 255}
#define BLACK        (tj_color) {.R = 0, .G = 0,.B = 0,.A = 0}
#define BRIGHT_RED   (tj_color) {.R = 255,.G = 0,.B = 0,.A = 0}
#define BRIGHT_BLUE  (tj_color) {.R = 0,.G = 0,.B = 255,.A = 0}
#define BRIGHT_GREEN (tj_color) {.R = 0,.G = 255,.B = 0,.A = 0}

#define hex_to_rgba(num) (tj_color) {.R = (num & 0xFF) / 255,.G = ((num >> 8) & 0xFF),.B = ((num >> 16) & 0xFF),.A = ((num >> 24) & 0xFF)}

#endif
