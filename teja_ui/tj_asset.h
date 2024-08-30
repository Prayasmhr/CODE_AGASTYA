#ifndef _TJ_ASSET_H
#define _TJ_ASSET_H

#include"heart_lib/lib.h"
#include"tj_vec2.h"

typedef struct tj_image tj_image;
typedef struct tj_font tj_font;

typedef struct tj_asset{
    string asset_tag;
    enum {
        IMAGE,
        FONT
    } type;
    union {
        tj_image* img_handle;
        tj_font* font_handle;
    } asset;
} tj_asset;

_BINARY_TREE_PROTOTYPE(tj_asset, asset)

extern asset_btree asset_tree;

tj_asset* asset_btree_search_by_tag(char* asset_tag);

#endif
