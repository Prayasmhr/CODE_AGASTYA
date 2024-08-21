#include"interpreter.h"
#include"../teja_ui/tj_uienv.h"
#include"../teja_ui/tj_button.h"

void interpret_fn_free(void* fn) {}

#define UINT_ARRAY_TO_COLOR(instruction_var) (tj_color){\
               .R = (u8)instruction_var->data.new_array.arr.uint32[0],\
               .G = (u8)instruction_var->data.new_array.arr.uint32[1],\
               .B = (u8)instruction_var->data.new_array.arr.uint32[2],\
               .A = (u8)instruction_var->data.new_array.arr.uint32[3]\
           }\


#define CHECK_ARRAY(instruction,_element_type,_size) \
   if(instruction.data.set_uientt_prop.data_type != UIENTT_PROP_DATA_TYPE_ARRAY) {\
       printf("%s property expects a array of %s [size: %d] \n",instruction.data.set_uientt_prop.prop.data,#_element_type,_size);\
   }\
   tjlang_ir_instruction* array_instruction = (tjlang_ir_instruction*)instruction.data.set_uientt_prop.value;\
   if(array_instruction->data.new_array.element_type != _element_type) {\
       printf("expected a array of %s \n",#_element_type);\
   }else if(array_instruction->data.new_array.size != _size) {\
       printf("expected a array with a size of %d\n", _size);\
   }\

typedef bool (*uientt_def_interpret_fn)(tjlang_interpret_state* ,tjlang_ir_zone* ,tjlang_ir_instruction ,tj_uientity**);

struct _interpreter_core_state {
   dynlib* app_module;
};

typedef struct _interpreter_type_fntable_entry {
   string key;
   int index;
   struct _interpreter_type_fntable_entry* next;
} _interpreter_type_fntable_entry;


// I know this is a very over complicated way to do this ,it could have been simpler but as i had OOP style for ui entity types, this is the best temporary i could think of.
// few days later i said temporary my ass!
// I think i will change the style of my ui entity types. That could make things better.
//

static void print_tag(tj_uienv env) {printf(" UIENV : %s\n",env.tag.data);}

struct _interpreter_type_fntable {
    uientt_def_interpret_fn interpret_fns[2];
    u16 size;
    _interpreter_type_fntable_entry** _entries;
};

struct _interpreter_uienv_process_state {
    u32 arr_size;
    tj_uientity** arr;
    string* tags;
};

struct _interpreter_iterate_state {
    u64 i;
    char ui_env[64];
    bool inside_uienv;
};

struct tjlang_interpret_state {
    struct _interpreter_core_state core;
    struct _interpreter_uienv_process_state uienv_process;
    struct _interpreter_type_fntable type_hashtable;
    struct _interpreter_iterate_state iterate;
};

static u16 hash(char *str,u16 capacity) {
   u32 hash = 5381;
   i32 c = 0;
   while ((c = *(str++)))
       hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
   return hash % capacity;
}

void _interpreter_type_fntable_insert(tjlang_interpret_state* interpret_state,char* tag,uientt_def_interpret_fn _interpret_fn) {
   u16 h_index = hash(tag,2);
   _interpreter_type_fntable_entry* _entry = malloc(sizeof(_interpreter_type_fntable_entry));
   u16 fn_index = interpret_state->type_hashtable.size;
   interpret_state->type_hashtable.interpret_fns[fn_index] = _interpret_fn;
   _entry->key = new_string(tag);
   _entry->index = fn_index;
   _entry->next = interpret_state->type_hashtable._entries[h_index];
   interpret_state->type_hashtable._entries[h_index] = _entry;
   _entry = NULL;
   interpret_state->type_hashtable.size++;
}

uientt_def_interpret_fn _interpreter_type_fntable_get(tjlang_interpret_state* interpret_state,char* tag) {
   u32 index = hash(tag,2);
   _interpreter_type_fntable_entry* entry = interpret_state->type_hashtable._entries[index];
   while (entry) {
       if(strncmp(entry->key.data,tag,entry->key.size) == 0)
           return interpret_state->type_hashtable.interpret_fns[entry->index];
       entry = entry->next;
   }
   return NULL;
}

void _interpreter_init(const void* ir,void** interpret_process_state) {
    *interpret_process_state = malloc(sizeof(tjlang_interpret_state));
    tjlang_interpret_state* interpret_state = (tjlang_interpret_state*)(*interpret_process_state);
    interpret_state->core.app_module = dynlib_load(get_app_module());
    snprintf(interpret_state->iterate.ui_env,64,"");
    interpret_state->iterate.inside_uienv = 0;

    interpret_state->uienv_process.arr_size = 0;
    interpret_state->uienv_process.arr = calloc(sizeof(tj_uientity), 32);
    interpret_state->uienv_process.tags = calloc(sizeof(string), 32);
    
    interpret_state->type_hashtable._entries = calloc(2,sizeof(_interpreter_type_fntable_entry));
    interpret_state->type_hashtable.size = 0;
    _interpreter_type_fntable_insert(interpret_state,"tj_button",_interpreter_interpret_button);
}

void* _interpreter_interpret_ir(const void* ir_data,void* interpret_process_state) {
   tjlang_ir* ir = (tjlang_ir*)ir_data;
   ll_node* zone_llnode = ir->zone_linkedlist->head;
   tjlang_interpret_state* interpret_state = (tjlang_interpret_state*)interpret_process_state;
   while (zone_llnode != NULL) {
       _interpreter_interpret_zone(interpret_state,(tjlang_ir_zone*)zone_llnode->data);
       zone_llnode = zone_llnode->next;
   }
   return NULL;
}

static void _interpreter_interpret_zone(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone) {
   interpret_state->iterate.i = 0;
   while (interpret_state->iterate.i < ir_zone->size) {
       switch (ir_zone->instructions[interpret_state->iterate.i].type) {
           case IR_ENV_DEFINTION:
               _interpreter_interpret_uienv_instruction(interpret_state,ir_zone,ir_zone->instructions[interpret_state->iterate.i]);
               break;
           case IR_UIENTT_DEFINITION:
               _interpreter_interpret_uientity_def(interpret_state,ir_zone,ir_zone->instructions[interpret_state->iterate.i]);
               break;
           case IR_LOAD_ASSET:
               if(ir_zone->instructions[interpret_state->iterate.i].data.load_asset.load_asset_type == LOAD_ASSET_IMAGE)
                   _interpreter_load_image_on_asset_tree(interpret_state,ir_zone,ir_zone->instructions[interpret_state->iterate.i]);
               else if(ir_zone->instructions[interpret_state->iterate.i].data.load_asset.load_asset_type == LOAD_ASSET_FONT)
                   _interpreter_load_font_on_asset_tree(interpret_state,ir_zone,ir_zone->instructions[interpret_state->iterate.i]);
               break;
           default:
               break;
       }
       interpret_state->iterate.i++;
   }
}

static void _interpreter_interpret_uienv_instruction(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction env_instruction) {
    if(env_instruction.data.env_definition.type == ENV_BEGIN) {
        if(interpret_state->iterate.inside_uienv)
            printf("environment nor ui entities can be nested\n");
        snprintf(interpret_state->iterate.ui_env,64,"%s",env_instruction.data.env_definition.tag.data);
        printf("env_name: %s\n",env_instruction.data.env_definition.tag.data);
        printf("tracking interpretation flow\n[TJ_UIENV]\n");
        interpret_state->iterate.inside_uienv = 1;
  }else if(env_instruction.data.env_definition.type == ENV_END) {
        printf("tracking interpretation flow [TJ_UIENV SIZE: %d]\n",env_instruction.data.env_definition.entities_defined);
        tj_uienv _uienv = tj_uienv_init(interpret_state->iterate.ui_env,env_instruction.data.env_definition.entities_defined,interpret_state->uienv_process.arr,interpret_state->uienv_process.tags);
        print_tag(_uienv);
        uienv_btree_insert(&uienv_tree, _uienv);
        uienv_tree.compare(NULL,NULL);
        interpret_state->iterate.inside_uienv = 0;
        interpret_state->uienv_process.arr_size = 0;
        free(interpret_state->uienv_process.arr);
        free(interpret_state->uienv_process.tags);
    }
}

static void _interpreter_load_image_on_asset_tree(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction load_image_instruction) {
   tj_asset image_asset = (tj_asset) {.type = IMAGE};
   image_asset.asset_tag = new_nstring(load_image_instruction.data.load_asset.tag.data,load_image_instruction.data.load_asset.tag.size);
   image_asset.asset.img_handle = render_engine.create_image(load_image_instruction.data.load_asset.path);
   interpret_state->iterate.i++;
   if(strncmp(ir_zone->instructions[interpret_state->iterate.i].data.set_asset_spec.spec.data,"c_dimension",13) == 0) 
       render_engine.set_image_size(image_asset.asset.img_handle,ir_zone->instructions[interpret_state->iterate.i].data.set_asset_spec.specs.image.dimensions);
   asset_btree_insert(&asset_tree, image_asset);
}

static void _interpreter_load_font_on_asset_tree(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction load_font_instruction) {
   tj_asset font_asset = (tj_asset) {.type = FONT,.asset_tag = load_font_instruction.data.load_asset.tag};
   font_asset.asset_tag = new_nstring(load_font_instruction.data.load_asset.tag.data,load_font_instruction.data.load_asset.tag.size);
   interpret_state->iterate.i++;
   u64 font_size = ir_zone->instructions[interpret_state->iterate.i].data.set_asset_spec.specs.font.font_size;
   font_asset.asset.font_handle = render_engine.create_font(load_font_instruction.data.load_asset.path,font_size);
   if(font_asset.asset.font_handle == NULL)
       printf("font could not be loaded successfully %llu\n",font_size);
   asset_btree_insert(&asset_tree, font_asset);
}

static void _interpreter_interpret_uientity_def(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction uientt_def_instruction) {
  if(uientt_def_instruction.data.uientt_definition.type != UIENTT_BEGIN) {
    printf("expected a [uientity_defintion_begin] instruction\n");
    return;
  }
  char* tag = uientt_def_instruction.data.uientt_definition.name.data;

  tj_uientity* ui_entity = NULL;
  tj_uientity_ginfo geometry_info;
  bool _error = 0;

  uientt_def_interpret_fn _interpret = _interpreter_type_fntable_get(interpret_state,uientt_def_instruction.data.uientt_definition.uientt_type.data);
  if(_interpret == NULL) {
    printf("invalid ui entity type %s\n",uientt_def_instruction.data.uientt_definition.uientt_type.data);
    return;
  }
  interpret_state->iterate.i++;
  while(ir_zone->instructions[interpret_state->iterate.i].type != IR_UIENTT_DEFINITION && ir_zone->instructions[interpret_state->iterate.i].data.uientt_definition.type != UIENTT_END){
    if(!_interpreter_interpret_geometry(interpret_state,ir_zone,ir_zone->instructions[interpret_state->iterate.i],&geometry_info)) {
      if(!_interpret(interpret_state,ir_zone,ir_zone->instructions[interpret_state->iterate.i],&ui_entity)) {
        printf("error interpreting \n");
        _error = 1;
      }
    }
    interpret_state->iterate.i++;
  }
  if(_error == 1) return;

  ui_entity->geometry_info = geometry_info;
  interpret_state->uienv_process.arr[interpret_state->uienv_process.arr_size] = ui_entity;
  interpret_state->uienv_process.tags[interpret_state->uienv_process.arr_size] = new_string(tag);
  interpret_state->uienv_process.arr_size++;

  ui_entity = NULL;
}

static bool _interpreter_interpret_geometry(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction setprop_instruction,tj_uientity_ginfo* ginfo) {
   if(setprop_instruction.type != IR_SET_UIENTT_PROPERTY) {
       printf("expected instruction to set ui entity property\n");
       return 0;
   }
   bool is_position = strncmp(setprop_instruction.data.set_uientt_prop.prop.data,"position",10) == 0;
   bool is_dimension = strncmp(setprop_instruction.data.set_uientt_prop.prop.data,"dimension",10) == 0;

   if(is_position || is_dimension) {
       CHECK_ARRAY(setprop_instruction,BASIC_LANG_TYPE_INTEGER,2);
       if (is_position) {
           ginfo->position.x = (array_instruction->data.new_array.arr.int32)[0];
           ginfo->position.y = (array_instruction->data.new_array.arr.int32)[1];
           return 1;
       }else if(is_dimension) {
           ginfo->dimension.x = (array_instruction->data.new_array.arr.int32)[0];
           ginfo->dimension.y = (array_instruction->data.new_array.arr.int32)[1];
           return 1;
       }
   }
   return 0;
}

static bool _interpreter_interpret_button(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction button_def,tj_uientity** uibutton) {
   if(ir_zone->instructions[interpret_state->iterate.i].type != IR_SET_UIENTT_PROPERTY) {
       printf("expected ui entity property assignment\n");
       return 0;
   }
   if(*uibutton == NULL) {
       printf("creating a button\n");
       *uibutton = (tj_uientity*)new_tj_button(NULL,(tj_color){0,0,0},(tj_color){0,0,0},NULL,NULL,new_string("")); 
   }
   tj_button button = (tj_button)(*uibutton);
   bool FG = strncmp(button_def.data.set_uientt_prop.prop.data,"fg",2) == 0;
   bool BG = strncmp(button_def.data.set_uientt_prop.prop.data,"bg",2) == 0;
   bool exit_code = 0;

   if(strncmp(button_def.data.set_uientt_prop.prop.data,"text_buffer",12) == 0 && button_def.data.set_uientt_prop.data_type == UIENTT_PROP_DATA_TYPE_STRING) {
       button->text_buffer = new_string((char*)button_def.data.set_uientt_prop.value);
       exit_code = 1;
   }else if(FG || BG) {
       // CHECK_ARRAY(button_def,BASIC_LANG_TYPE_UINTEGER,4);
       if(button_def.data.set_uientt_prop.data_type != UIENTT_PROP_DATA_TYPE_ARRAY)
           printf("%s property expects a array of %s [size: %d] \n",button_def.data.set_uientt_prop.prop.data,"BASIC_LANG_TYPE_INTEGER",4);
       
       tjlang_ir_instruction* array_instruction = (tjlang_ir_instruction*)button_def.data.set_uientt_prop.value;
       if(array_instruction->data.new_array.element_type != BASIC_LANG_TYPE_UINTEGER)
           printf("expected a array of %s \n","BASIC_LANG_TYPE_INTEGER");
       else if(array_instruction->data.new_array.size != 4)
           printf("expected a array with a size of %d\n", 4);
       
       for(int i = 0;i < array_instruction->data.new_array.size; i++)
           printf("%d\n", array_instruction->data.new_array.arr.uint32[i]);
       
       if (FG){
           button->fg = (tj_color) {
               .R = (u8)array_instruction->data.new_array.arr.uint32[0],
               .G = (u8)array_instruction->data.new_array.arr.uint32[1],
               .B = (u8)array_instruction->data.new_array.arr.uint32[2],
               .A = (u8)array_instruction->data.new_array.arr.uint32[3],
           };
       }else if(BG) {
           button->bg = (tj_color) {
               .R = (u8)array_instruction->data.new_array.arr.uint32[0],
               .G = (u8)array_instruction->data.new_array.arr.uint32[1],
               .B = (u8)array_instruction->data.new_array.arr.uint32[2],
               .A = (u8)array_instruction->data.new_array.arr.uint32[3],
           };
       }
       exit_code = 1;
   }else if(strncmp(button_def.data.set_uientt_prop.prop.data,"font",5) == 0) {
       if(button_def.data.set_uientt_prop.data_type != UIENTT_PROP_DATA_TYPE_IDENTIFIER)
           printf("expected a identifier\n");
       
       char* f_asset_tag = (char*)button_def.data.set_uientt_prop.value;
       tj_asset* f_asset = asset_btree_search_by_tag(f_asset_tag);
       if(f_asset == NULL)
           printf("asset tree doesnt have the font by the given tag %s\n",button_def.data.set_uientt_prop.prop.data);
       else if(f_asset->type != FONT)
           printf("searched asset [%s] is not a font\n",f_asset_tag);
       
       button->font = f_asset->asset.font_handle;
       exit_code = 1;
   }else if(strncmp(button_def.data.set_uientt_prop.prop.data,"image",6) == 0) {
       if(button_def.data.set_uientt_prop.data_type != UIENTT_PROP_DATA_TYPE_IDENTIFIER)
           printf("expected a identifier\n");
       
       char* img_asset_tag = (char*)button_def.data.set_uientt_prop.value;
       tj_asset* img_asset = asset_btree_search_by_tag(img_asset_tag);
       if(img_asset == NULL)
           printf("asset tree doesnt have the image by the given tag %s\n",button_def.data.set_uientt_prop.prop.data);
       else if(img_asset->type != IMAGE)
           printf("searched asset [%s] is not a image\n",img_asset_tag);
       
       button->image = img_asset->asset.img_handle;
       exit_code = 1;
   }else if(strncmp(button_def.data.set_uientt_prop.prop.data, "callback",9) == 0) {
       if(button_def.data.set_uientt_prop.data_type != UIENTT_PROP_DATA_TYPE_IDENTIFIER)
           printf("expected a identifier\n");
       
       button->callback = (btn_callback)dynlib_symbol(interpret_state->core.app_module, (char*)button_def.data.set_uientt_prop.value);
       exit_code = 1;
   }
   *uibutton = (tj_uientity*)button;
   return exit_code;
}

static bool _interpreter_interpret_label(tjlang_interpret_state* interpret_state,tjlang_ir_zone* ir_zone,tjlang_ir_instruction label_def,tj_uientity** label) {
   if(ir_zone->instructions[interpret_state->iterate.i].type != IR_SET_UIENTT_PROPERTY) {
       printf("expected ui entity property assignment\b");
       return 0;
   }
   return 1;
}

void _interpreter_shutdown(void* ir_data,void* interpret_process_state) {
   tjlang_interpret_state* interpret_state = (tjlang_interpret_state*)interpret_process_state;
   _ir_free(ir_data);
   dynlib_unload(interpret_state->core.app_module);
}
