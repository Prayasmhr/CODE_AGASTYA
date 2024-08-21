#include"instruction_representation.h"

tjlang_ir* _ir_init(void)
{
    tjlang_ir* ir = malloc(sizeof(tjlang_ir));
    ir->zone_linkedlist = linkedlist_create(sizeof(tjlang_ir_zone));
    return ir;
}

tjlang_ir_zone *_ir_create_zone(u8 capacity)
{
    tjlang_ir_zone* zone = malloc(sizeof(tjlang_ir_zone));
    zone->capacity = capacity;
    zone->size = 0;
    zone->instructions = malloc(sizeof(tjlang_ir_instruction) * capacity);	
    return zone;
}

void _ir_free_zone(void* zone) {
    tjlang_ir_zone* ir_zone = (tjlang_ir_zone*)zone;
    free(ir_zone->instructions);
    free(ir_zone);
}

void _ir_push_zone(tjlang_ir* ir,tjlang_ir_zone* zone) {
    linkedlist_insert(ir->zone_linkedlist, (void*)zone);
}

void _ir_zone_push_instruction(tjlang_ir_zone* zone, tjlang_ir_instruction instruction) {
    if(zone->size + 1 >= zone->capacity) {
        zone->capacity += 4;
        zone->instructions = malloc(sizeof(tjlang_ir_instruction) * zone->capacity);
    }
    if(instruction.type == IR_SET_UIENTT_PROPERTY && instruction.data.set_uientt_prop.data_type == UIENTT_PROP_DATA_TYPE_ARRAY) {
        tjlang_ir_instruction* array_instruction = (tjlang_ir_instruction*)instruction.data.set_uientt_prop.value;
        for(int i = 0;i < array_instruction->data.new_array.size; i++) {
            if(array_instruction->data.new_array.element_type == BASIC_LANG_TYPE_UINTEGER) {
                printf("%u\n",array_instruction->data.new_array.arr.uint32[i]);
            }
        }
        printf("uinteger array\n");
    }
    zone->instructions[zone->size++] = instruction;
}

void _ir_clear(tjlang_ir* ir) {
   linkedlist_destroy(ir->zone_linkedlist,_ir_free_zone);
   ir->zone_linkedlist = linkedlist_create(sizeof(tjlang_ir_zone));
}

void _ir_free(tjlang_ir* ir) {
   linkedlist_destroy(ir->zone_linkedlist,_ir_free_zone); 
   free(ir);
}
