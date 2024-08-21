#ifndef _TJ_APPSTATE_H
#define _TJ_APPSTATE_H

#include"stdbool.h"

typedef struct _tj_appobject _tj_appobject;
typedef struct tj_appstate_t{
    _tj_appobject* inst;
    bool running;
} tj_appstate;

#endif
