
/*** Elan di Andrea Giotti ***/

#define VER     "1.00"
#define TITLE   "Elan by Andrea Giotti, Version "VER" ("__DATE__")"
#define NDEBUG

#include <assert.h>

typedef char boolean;   /* Per evitare conflitti con tipo bool di ncurses */

#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    (!FALSE)
#endif

typedef double real;

#ifndef PI
#define PI      3.14159265358979323846
#endif
#ifndef max
#define max(x, y)   (((x) > (y))? (x) : (y))
#endif
#ifndef min
#define min(x, y)   (((x) < (y))? (x) : (y))
#endif

#define round(f)    (((f) - floor(f) < 0.5)? (int)floor(f) : (int)floor(f) + 1)
#define random(f)   ((((real)(f)) / RAND_MAX) * rand())

typedef enum error_class
    {
    ok,
    console_err,
    read_file_err,
    write_file_err,
    ERROR_CLASS_NUMBER
    } error_class;

