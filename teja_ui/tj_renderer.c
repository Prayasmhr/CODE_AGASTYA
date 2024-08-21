#include "tj_renderer.h"
#include "../heart_lib/ht_dynlib.h"
#include "../heart_lib/ht_string.h"

tj_render_engine render_engine;
asset_btree asset_tree;
static dynlib *render_lib;

i32 asset_compare(const tj_asset *lhs, const tj_asset *rhs)
{
    return string_compare(lhs->asset_tag, rhs->asset_tag);
}

void asset_destroy(tj_asset *asset)
{
    switch (asset->type)
    {
    case IMAGE:
        render_engine.destroy_image(asset->asset.img_handle);
        break;
    case FONT:
        render_engine.destroy_font(asset->asset.font_handle);
        break;
    }
    string_free(&asset->asset_tag);
    free(asset);
}

void tj_render_load_backend(tj_render_backend_type backend_type)
{
    render_lib = dynlib_load(backend_type == RAY_RENDERER ? "ray_renderer.dll" : "photon8_renderer.dll");

    render_engine.init = (tj_render_engine_init_fn)dynlib_symbol(render_lib, "ray_renderer_init");
    render_engine.shutdown = (tj_render_engine_shutdown_fn)dynlib_symbol(render_lib, "ray_renderer_shutdown");

    render_engine.create_image = (tj_render_engine_create_image_fn)dynlib_symbol(render_lib, "ray_renderer_create_image");
    render_engine.destroy_image = (tj_render_engine_destroy_image_fn)dynlib_symbol(render_lib, "ray_renderer_destroy_image");
    render_engine.create_font = (tj_render_engine_create_font_fn)dynlib_symbol(render_lib, "ray_renderer_create_font");
    render_engine.destroy_font = (tj_render_engine_destroy_font_fn)dynlib_symbol(render_lib, "ray_renderer_destroy_font");

    render_engine.clear = (tj_render_engine_clear_fn)dynlib_symbol(render_lib, "ray_renderer_clear");
    render_engine.flip = (tj_render_engine_flip_fn)dynlib_symbol(render_lib, "ray_renderer_flip");

    render_engine.render_image = (tj_render_engine_render_image_fn)dynlib_symbol(render_lib, "ray_renderer_render_image");
    render_engine.render_rect = (tj_render_engine_render_rect_fn)dynlib_symbol(render_lib, "ray_renderer_render_rect");
    render_engine.render_circle = (tj_render_engine_render_circle_fn)dynlib_symbol(render_lib, "ray_renderer_render_circle");
    render_engine.render_line = (tj_render_engine_render_line_fn)dynlib_symbol(render_lib, "ray_renderer_render_line");
    render_engine.render_text = (tj_render_engine_render_text_fn)dynlib_symbol(render_lib, "ray_renderer_render_text");

    render_engine.get_font_size = (tj_render_engine_get_font_size_fn)dynlib_symbol(render_lib, "ray_renderer_get_font_size");
    render_engine.get_image_size = (tj_render_engine_get_image_size_fn)dynlib_symbol(render_lib, "ray_renderer_get_image_size");
    render_engine.set_image_size = (tj_render_engine_set_image_size_fn)dynlib_symbol(render_lib, "ray_renderer_set_image_size");

    asset_tree = asset_btree_init(asset_compare, asset_destroy);
}

tj_asset *asset_btree_search_by_tag(char *asset_tag)
{
    tj_asset search = (tj_asset){.asset_tag = new_string(asset_tag)};
    tj_asset *out = asset_btree_search(&asset_tree,search);
    if (out == NULL)
    {
        printf("could not find font\n");
    }
    return out;
}

void tj_render_unload_backend(tj_render_backend_type backend_type)
{
    dynlib_unload(render_lib);
    asset_btree_free(&asset_tree);
}

_BINARY_TREE_IMPLEMENT(tj_asset, asset)
