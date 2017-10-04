#include "PatchLev.h"
char __Ident_pml[] = "$PatchLevel: atariST/TT ("
#if defined __mcoldfire__
"cfv4e"
#elif !defined (__M68881__) && !defined (sfp004)
"68000"
#else
"68020-60"
#endif
#ifdef __MSHORT__
"/mshort"
#endif
") PML: " PatchLevel " $";
