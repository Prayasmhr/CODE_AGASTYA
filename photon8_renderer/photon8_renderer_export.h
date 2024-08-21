#ifndef _PHOTON8_RENDER_EXPORT_H
#define _PHOTON8_RENDER_EXPORT_H

#ifdef _WIN32
    #define PHOTON8_RENDERER_API __declspec(dllexport)
#else
    #error "unsupported platform"
#endif

#endif