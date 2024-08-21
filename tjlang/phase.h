#ifndef _TJLANG_PHASE_H
#define _TJLANG_PHASE_H

typedef void    (*phase_init)       (const void*,void**);
typedef void*   (*phase_process)    (const void*,void*);
typedef void    (*phase_reset)      (const void*,void*);
typedef void    (*phase_cleanup)       (void*,void*);

typedef struct phase_chain_node {
    struct phase_reversed_chain_node* next;
    void* data_handle;
} phase_chain_node;

typedef struct tjlang_phase_interface {
    phase_init _init;
    phase_process _process;
    phase_reset _reset;
    phase_cleanup _cleanup;
} tjlang_phase_interface;

#endif
