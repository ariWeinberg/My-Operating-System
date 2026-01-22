// ===== FILE: STD/defs.h =====
#ifndef STD_DEFS_H
#define STD_DEFS_H

#include "int.h"

// NULL pointer
#ifndef NULL
#define NULL ((void*)0)
#endif

// offsetof macro
#define offsetof(type, member) ((size_t) &(((type*)0)->member))

#endif
