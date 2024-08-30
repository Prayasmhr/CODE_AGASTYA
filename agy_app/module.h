#ifndef _AGY_MODULE_H
#define _AGY_MODULE_H

#define _EXPORT __declspec(dllexport)
#include"../teja_ui/lib.h"
#include"time.h"

struct _tj_appobject {
    string message;
    tj_uienv* active_ui_env; 
    char* ative_uienv_tag;
    tj_font* font;
};

#endif
