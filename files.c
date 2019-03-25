
#include "global.h"
#include "coredefs.h"
#include "texts.h"
#include "symbols.h"
#include "elan.h"
#include "interf.h"
#include "system.h"
#include "files.h"

const char internal_header[HEADER_LEN] = HEADER_TAG;
char file_header[HEADER_LEN];

void save_game()
    {
    if(save_state() != ok)
        display_message(save_error);
    else
        display_message(game_saved);
    }

void load_game()
    {
    if(load_state() != ok)
        display_message(load_error);
    else
        {
        if(strcmp(internal_header, file_header))
            display_message(file_mismatch);
        else
            display_message(game_loaded);
        }

    display_map();
    if(ifc.mode != start_mode)
        display_frame();
    }

error_class save_state()
    {
    FILE *fp;
    device *plant;
    channel *duct;
    mobile *turtle;
    cell *loc;
    int p, d, g, t;

    fp = fopen(SAVEFILE_NAME, "wb");
    if(!fp)
        return write_file_err;

    fwrite(internal_header, HEADER_LEN, 1, fp);
    fwrite(&env, sizeof(env), 1, fp);
    fwrite(&sys, sizeof(sys), 1, fp);
    fwrite(&usr, sizeof(usr), 1, fp);
    fwrite(&ifc, sizeof(ifc), 1, fp);

    for(p = 0; p < sys.tot_plants; p++)
        {
        plant = sys.plant[p];
        fwrite(plant, sizeof(*plant), 1, fp);
        write_cell_ref(plant->loc, fp);
        }

    for(d = 0; d < sys.tot_ducts; d++)
        {
        duct = sys.duct[d];
        fwrite(duct, sizeof(*duct), 1, fp);
        write_cell_ref(duct->start_loc, fp);
        write_cell_ref(duct->end_loc, fp);
        write_cell_ref(duct->break_loc, fp);
        for(p = 0; p < MAX_PROVIDERS_NUMBER; p++)
            {
            write_device_ref(duct->provider[p], fp);
            if(!duct->provider[p])
                break;
            }
        }

    for(p = 0; p < sys.tot_plants; p++)
        for(g = 0; g < PRIMARY_RT_NUMBER; g++)
            write_channel_ref(sys.plant[p]->input[g], fp);

    for(t = 0; t < sys.tot_turtles; t++)
        {
        turtle = sys.turtle[t];
        if(turtle)
            {
            fwrite(turtle, sizeof(*turtle), 1, fp);
            write_cell_ref(turtle->loc, fp);
            }
        }

    for(loc = &env.map[0][0]; loc <= &env.map[MAP_ROWS - 1][MAP_COLS - 1]; loc++)
        {
        write_channel_ref(loc->alfa_duct, fp);
        write_channel_ref(loc->beta_duct, fp);
        write_cell_ref(loc->alfa_flow, fp);
        write_cell_ref(loc->beta_flow, fp);
        write_cell_ref(loc->break_flow, fp);
        }

    write_cell_ref(ifc.cursor, fp);

    fclose(fp);

    return ok;
    }

error_class load_state()
    {
    FILE *fp;
    device *plant;
    channel *duct;
    mobile *turtle;
    cell *loc;
    int p, d, g, t;

    fp = fopen(SAVEFILE_NAME, "rb");
    if(!fp)
        return read_file_err;

    for(p = 0; p < sys.tot_plants; p++)
        free(sys.plant[p]);

    for(d = 0; d < sys.tot_ducts; d++)
        free(sys.duct[d]);

    for(t = 0; t < sys.tot_turtles; t++)
        free(sys.turtle[t]);

    fread(file_header, HEADER_LEN, 1, fp);
    fread(&env, sizeof(env), 1, fp);
    fread(&sys, sizeof(sys), 1, fp);
    fread(&usr, sizeof(usr), 1, fp);
    fread(&ifc, sizeof(ifc), 1, fp);

    for(p = 0; p < sys.tot_plants; p++)
        {
        plant = calloc(1, sizeof(*plant));
        assert(plant != NULL);
        sys.plant[p] = plant;
        fread(plant, sizeof(*plant), 1, fp);
        loc = read_cell_ref(fp);
        plant->loc = loc;
        loc->plant = plant;
        }

    for(d = 0; d < sys.tot_ducts; d++)
        {
        duct = calloc(1, sizeof(*duct));
        assert(duct != NULL);
        sys.duct[d] = duct;
        fread(duct, sizeof(*duct), 1, fp);
        duct->start_loc = read_cell_ref(fp);
        duct->end_loc = read_cell_ref(fp);
        duct->break_loc = read_cell_ref(fp);
        for(p = 0; p < MAX_PROVIDERS_NUMBER; p++)
            {
            duct->provider[p] = read_device_ref(fp);
            if(!duct->provider[p])
                break;
            }
        }

    for(p = 0; p < sys.tot_plants; p++)
        for(g = 0; g < PRIMARY_RT_NUMBER; g++)
            sys.plant[p]->input[g] = read_channel_ref(fp);

    for(t = 0; t < sys.tot_turtles; t++)
        {
        if(sys.turtle[t])
            {
            turtle = calloc(1, sizeof(*turtle));
            assert(turtle != NULL);
            sys.turtle[t] = turtle;
            fread(turtle, sizeof(*turtle), 1, fp);
            loc = read_cell_ref(fp);
            turtle->loc = loc;
            loc->turtle = turtle;
            }
        }

    for(loc = &env.map[0][0]; loc <= &env.map[MAP_ROWS - 1][MAP_COLS - 1]; loc++)
        {
        loc->alfa_duct = read_channel_ref(fp);
        loc->beta_duct = read_channel_ref(fp);
        loc->alfa_flow = read_cell_ref(fp);
        loc->beta_flow = read_cell_ref(fp);
        loc->break_flow = read_cell_ref(fp);
        }

    ifc.cursor = read_cell_ref(fp);

    fclose(fp);

    return ok;
    }

size_t write_device_ref(device *plant, FILE *fp)
    {
    int p;

    if(plant)
        for(p = 0; sys.plant[p] != plant; p++);
    else
        p = NULL_REF;

    return fwrite(&p, sizeof(p), 1, fp);
    }

device *read_device_ref(FILE *fp)
    {
    int p;

    fread(&p, sizeof(p), 1, fp);
    if(p != NULL_REF)
        return sys.plant[p];
    else
        return NULL;
    }

size_t write_channel_ref(channel *duct, FILE *fp)
    {
    int d;

    if(duct)
        for(d = 0; sys.duct[d] != duct; d++);
    else
        d = NULL_REF;

    return fwrite(&d, sizeof(d), 1, fp);
    }

channel *read_channel_ref(FILE *fp)
    {
    int d;

    fread(&d, sizeof(d), 1, fp);
    if(d != NULL_REF)
        return sys.duct[d];
    else
        return NULL;
    }

size_t write_mobile_ref(mobile *turtle, FILE *fp)
    {
    int t;

    if(turtle)
        for(t = 0; sys.turtle[t] != turtle; t++);
    else
        t = NULL_REF;

    return fwrite(&t, sizeof(t), 1, fp);
    }

mobile *read_mobile_ref(FILE *fp)
    {
    int t;

    fread(&t, sizeof(t), 1, fp);
    if(t != NULL_REF)
        return sys.turtle[t];
    else
        return NULL;
    }

size_t write_cell_ref(cell *loc, FILE *fp)
    {
    position pos;

    if(loc)
        pos = loc->pos;
    else
        pos.i = NULL_REF;

    return fwrite(&pos, sizeof(pos), 1, fp);
    }

cell *read_cell_ref(FILE *fp)
    {
    position pos;

    fread(&pos, sizeof(pos), 1, fp);
    if(pos.i != NULL_REF)
        return &env.map[pos.i][pos.j];
    else
        return NULL;
    }

error_class dump_map()
    {
    FILE *fp;
    int i, j;

    fp = fopen(DUMPFILE_NAME, "w");
    if(!fp)
        return write_file_err;

    for(i = 0; i < MAP_ROWS; i++)
        {
        for(j = 0; j < MAP_COLS; j++)
            fputc(env.map[i][j].sym.icon, fp);
        fputc('\n', fp);
        }

    fclose(fp);

    return ok;
    }

