#ifndef _HT_EXPORT_H
#define _HT_EXPORT_H

#ifdef _WIN32
    #ifdef _HT_BUILD
        #define _HT_LIB_API __declspec(dllexport)
    #else 
        #define _HT_LIB_API __declspec(dllimport)
    #endif
#endif

#endif
