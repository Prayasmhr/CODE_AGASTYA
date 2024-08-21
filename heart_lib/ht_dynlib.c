#ifdef _WIN32

#include"ht_dynlib.h"
#include<stdio.h>

struct dynlib {
    HINSTANCE lib_inst;
};

dynlib* dynlib_load(char* path) {
    dynlib *lib = malloc(sizeof(dynlib));
    printf("loading dynamic library %s\n",path);
    lib->lib_inst = LoadLibrary(path);
    if(!lib->lib_inst){
        printf("could not load dynamic library \n");
    }
    return lib;
}

FARPROC dynlib_symbol(const dynlib* lib,char* symbol) {
    printf("loading symbol %s\n",symbol);
    FARPROC res = GetProcAddress(lib->lib_inst, symbol);
    u32 err_code = GetLastError();
    if(err_code != 0) {
        printf("%s %lu\n",symbol, GetLastError());
    }
    return res;       
}

void dynlib_unload(dynlib* lib) {
    printf("unloading the dll\n");
    FreeLibrary(lib->lib_inst);
    free(lib);
}

#endif
