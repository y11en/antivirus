#ifndef VER_MOD_H_INCLUDED_6C6403BC_7106_4b10_91AA_9B387C64584E
#define VER_MOD_H_INCLUDED_6C6403BC_7106_4b10_91AA_9B387C64584E
    // updater version which will be output to trace file
    #define UPD_VER_MAIN    "pre 1.5.005"

    // this identifier is requested when compiling resources
    #define VER_INTERNALNAME_STR        "updater"

    // special build for dll and executable identification
    #define VER_SPECIALBUILD_STR         UPD_VER_MAIN

#if (defined _WIN32) || (defined WIN32) || (defined(__APPLE__))
    #include "Version/ver_product.h"
#else
    #include "include/automate.h"
#endif

#endif // VER_MOD_H_INCLUDED_6C6403BC_7106_4b10_91AA_9B387C64584E
