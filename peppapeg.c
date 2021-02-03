#include <stdio.h>
#include "peppapeg.h"

P4_PUBLIC(P4_String)
P4_Version(void) {
    static char version[15];
    sprintf(version, "%i.%i.%i", P4_MAJOR_VERSION, P4_MINOR_VERSION, P4_PATCH_VERSION);
    return version;
}
