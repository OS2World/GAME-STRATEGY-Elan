
#include "global.h"
#include "coredefs.h"
#include "texts.h"
#include "symbols.h"
#include "elan.h"
#include "interf.h"
#include "config.h"
#include "console.h"

configuration cfg;

error_class read_config()
    {
    FILE *fp;
    style_type style;
    color_type color;
    colors_pair pair;

    fp = fopen(CONFIGFILE_NAME, "r");
    if(!fp)
        return read_file_err;

    fscanf(fp, "%d%*[^\n]", &cfg.output_mode);
    fscanf(fp, "%d%*[^\n]", &cfg.console_width);
    fscanf(fp, "%d%*[^\n]", &cfg.console_height);
    for(style = 0; style < STYLE_TYPE_NUMBER; style++)
        {
        fscanf(fp, "%d,%d%*[^\n]", &pair.fg, &pair.bg);
        cfg.style_pen[style] = register_pen(pair);
        }
    for(color = 0; color < COLOR_TYPE_NUMBER; color++)
        {
        fscanf(fp, "%d,%d%*[^\n]", &pair.fg, &pair.bg);
        cfg.color_pen[color] = register_pen(pair);
        }

    fclose(fp);

    return ok;
    }

