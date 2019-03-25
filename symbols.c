
#include "global.h"
#include "coredefs.h"
#include "symbols.h"

void set_icon(symbol *sp, icon_type icon)
    {
    if(sp->icon != icon)
        {
        sp->icon = icon;
        sp->updated = TRUE;
        }
    }

void set_color(symbol *sp, color_type color)
    {
    if(sp->color != color)
        {
        sp->color = color;
        sp->updated = TRUE;
        }
    }

void set_attribute(symbol *sp, attribute_type attribute)
    {
    if(sp->attribute != attribute)
        {
        sp->attribute = attribute;
        sp->updated = TRUE;
        }
    }

