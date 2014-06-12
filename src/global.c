#include "global.h"


#include <stdio.h>
#include <stdarg.h>

void debug_print(int dbg_lvl, char *fmt, ...){    
#ifdef DEBUG
    va_list argptr;             
  
    va_start(argptr,fmt);
    printf(fmt, argptr);
    va_end(argptr);
#endif
}