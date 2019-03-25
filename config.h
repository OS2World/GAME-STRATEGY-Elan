
#include <stdio.h>

#define CONFIGFILE_NAME     "elan.cfg"

typedef struct configuration
    {
    int output_mode;
    int console_width;
    int console_height;
    int style_pen[STYLE_TYPE_NUMBER];
    int color_pen[COLOR_TYPE_NUMBER];
    } configuration;

extern configuration cfg;

/* Protos */
error_class read_config(void);

