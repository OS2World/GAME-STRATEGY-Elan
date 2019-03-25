
#include "global.h"
#include "coredefs.h"
#include "params.h"
#include "texts.h"
#include "symbols.h"
#include "elan.h"
#include "interf.h"
#include "config.h"
#include "console.h"

interface ifc;

const char dir_key[DIRECTION_NUMBER] = {'8', '2', '4', '6', '7', '1', '9', '3'};

void open_io()
    {
    error_class error;

    error = read_config();
    if(error)
        {
        fprintf(stderr, "Unable to read " CONFIGFILE_NAME "\n");
        exit(error);
        }

    error = open_console(TITLE, 0, 0, cfg.console_width, cfg.console_height);
    if(error)
        {
        fprintf(stderr, "Unable to open console\n");
        exit(error);
        }

    init_interface();

    hide_cursor();
    set_style(standard_style);
    clear_screen();
    }

void close_io(void)
    {
    show_cursor();
    close_console();
    }

void init_interface()
    {
    ifc.mode = start_mode;
    ifc.cursor = &env.map[MAP_ROWS / 2][MAP_COLS / 2];
    ifc.history.last = 0;
    ifc.history.clear_message = FALSE;
    }

user_command query_for_command(event news)
    {
    user_command command;
    char input_char;
    direction dir;
    device *active_plant;
    channel *active_duct;
    int p;

    switch(news)
        {
        case no_news:
        break;

        case extension_failure:
        ifc.mode = control_mode;
        break;

        case time_passed:
        break;

        case shuttle_arrival:
        ifc.mode = trade_mode;
        break;

        case shuttle_departure:
        ifc.mode = control_mode;
        break;

        case colony_end:
        ifc.mode = watch_mode;
        break;

        default:
        assert(0);
        break;
        }

    command.opcode = cmd_nop;
    do
        {
        display_mode(ifc.mode);
        if(news > PRIMARY_EVENT_NUMBER)
            news = no_news;
        else
            display_zoom(ifc.cursor);

        locate(ifc.cursor->pos.i, ifc.cursor->pos.j);
        show_cursor();
        input_char = read_char();
        hide_cursor();

        if(ifc.history.clear_message)
            display_message(blank_message);

        switch(input_char)
            {
            case ' ':
            switch(ifc.mode)
                {
                case trade_mode:
                command.opcode = cmd_day;
                break;

                case start_mode:
                command.opcode = cmd_reset;
                break;

                default:
                break;
                }
            break;

            case '0':
            switch(ifc.mode)
                {
                case start_mode:
                command.opcode = cmd_start;
                command.loc_arg = ifc.cursor;
                break;

                case trade_mode:
                display_message(confirm_departure);
                break;

                default:
                command.opcode = cmd_day;
                break;
                }
            break;

            case '\t':
            if(ifc.mode != start_mode)
                command.opcode = cmd_forward;
            break;

            case '>':
            if(ifc.mode != start_mode && ifc.mode != trade_mode)
                command.opcode = cmd_year;
            break;

            case '[':
            switch(ifc.mode)
                {
                case control_mode:
                command.opcode = cmd_add_dev;
                command.loc_arg = ifc.cursor;
                break;

                case trade_mode:
                command.opcode = cmd_buy_dev;
                break;

                default:
                break;
                }
            break;

            case ']':
            switch(ifc.mode)
                {
                case control_mode:
                command.opcode = cmd_add_mob;
                command.loc_arg = ifc.cursor;
                break;

                case trade_mode:
                command.opcode = cmd_buy_mob;
                break;

                default:
                break;
                }
            break;

            case 'x':
            if(ifc.mode == control_mode)
                {
                if(ifc.cursor->plant)
                    {
                    command.opcode = cmd_del_dev;
                    command.loc_arg = ifc.cursor;
                    }
                else
                    if(ifc.cursor->alfa_duct)
                        {
                        command.opcode = cmd_del_chl;
                        command.loc_arg = ifc.cursor;
                        }
                }
            break;

            case '\r':
            if(ifc.cursor->plant)
                {
                if(ifc.mode == control_mode)
                    {
                    command.opcode = cmd_switch_dev;
                    command.loc_arg = ifc.cursor;
                    }
                }
            else
                if(ifc.cursor->alfa_duct && ifc.cursor->beta_duct)
                    {
                    command.opcode = cmd_swap_chl;
                    command.loc_arg = ifc.cursor;
                    }
            break;

            case '+':
            if(ifc.cursor->plant)
                {
                switch(ifc.mode)
                    {
                    case control_mode:
                    command.opcode = cmd_set_dev;
                    command.loc_arg = ifc.cursor;
                    command.num_arg =
                        ifc.cursor->plant->target_output / ifc.cursor->plant->max_state
                        + KNOB_RESOLUTION;
                    break;

                    case trade_mode:
                    if(ifc.cursor->plant->max_capacity)
                        {
                        command.opcode = cmd_trade_dev;
                        command.loc_arg = ifc.cursor;
                        command.num_arg =
                            ifc.cursor->plant->current_capacity / ifc.cursor->plant->max_capacity
                            + KNOB_RESOLUTION;
                        }
                    break;

                    default:
                    break;
                    }
                }
            else
                if(ifc.cursor->turtle && ifc.mode == control_mode)
                    {
                    command.opcode = cmd_set_mob;
                    command.loc_arg = ifc.cursor;
                    command.num_arg = ifc.cursor->turtle->mcp.tag + 1;
                    }
            break;

            case '-':
            if(ifc.cursor->plant)
                {
                switch(ifc.mode)
                    {
                    case control_mode:
                    command.opcode = cmd_set_dev;
                    command.loc_arg = ifc.cursor;
                    command.num_arg =
                        ifc.cursor->plant->target_output / ifc.cursor->plant->max_state
                        - KNOB_RESOLUTION;
                    break;

                    case trade_mode:
                    if(ifc.cursor->plant->max_capacity)
                        {
                        command.opcode = cmd_trade_dev;
                        command.loc_arg = ifc.cursor;
                        command.num_arg =
                            ifc.cursor->plant->current_capacity / ifc.cursor->plant->max_capacity
                            - KNOB_RESOLUTION;
                        }
                    break;

                    default:
                    break;
                    }
                }
            else
                if(ifc.cursor->turtle && ifc.mode == control_mode)
                    {
                    command.opcode = cmd_set_mob;
                    command.loc_arg = ifc.cursor;
                    command.num_arg = ifc.cursor->turtle->mcp.tag - 1;
                    }
            break;

            case '*':
            if(ifc.cursor->plant)
                {
                switch(ifc.mode)
                    {
                    case control_mode:
                    command.opcode = cmd_set_dev;
                    command.loc_arg = ifc.cursor;
                    if(ifc.cursor->plant->target_output)
                        command.num_arg = 0;
                    else
                        command.num_arg = 1;
                    break;

                    case trade_mode:
                    if(ifc.cursor->plant->max_capacity)
                        {
                        command.opcode = cmd_trade_dev;
                        command.loc_arg = ifc.cursor;
                        if(ifc.cursor->plant->current_capacity)
                            command.num_arg = 0;
                        else
                            command.num_arg = 1;
                        }
                    break;

                    default:
                    break;
                    }
                }
            else
                if(ifc.cursor->turtle && ifc.mode == control_mode)
                    {
                    command.opcode = cmd_set_mob;
                    command.loc_arg = ifc.cursor;
                    command.num_arg = survey;
                    }
            break;

            case 'l':
            command.opcode = cmd_load;
            break;

            case 's':
            command.opcode = cmd_save;
            break;

            case 'd':
            command.opcode = cmd_dump;
            break;

            case 'q':
            if(ifc.mode != start_mode)
                command.opcode = cmd_quit;
            break;

            case ESC_KEY:
            command.opcode = cmd_esc;
            break;

            case '/':
            if(ifc.mode == install_channel_mode || ifc.mode == extend_channel_mode)
                ifc.mode = control_mode;
            else
                if(ifc.mode == control_mode)
                    {
                    if(ifc.cursor->plant)
                        ifc.mode = install_channel_mode;
                    else
                        if(ifc.cursor->alfa_duct)
                            ifc.mode = extend_channel_mode;
                    }
            break;

            case '$':
            display_stats();
            pause();
            break;

            case BACKSPACE_KEY:
            display_history();
            pause();
            break;

            case '?':
            if(ifc.mode != start_mode)
                {
                display_message(title_message);
                display_help();
                pause();
                display_message(blank_message);
                }
            else
                display_message(place_habitat);
            break;

            case 'r':
            redraw_screen();
            break;

            case '.':
            if(ifc.mode != install_channel_mode && ifc.mode != extend_channel_mode)
                if(sys.tot_plants)
                    {
                    for(p = 0; p < sys.tot_plants && sys.plant[p] != ifc.cursor->plant; p++);

                    if(p == sys.tot_plants)
                        p = 0;

                    active_plant = sys.plant[p];
                    do
                        {
                        p++;
                        if(p == sys.tot_plants)
                            p = 0;

                        if(!sys.plant[p]->demand_driven || sys.plant[p]->broken)
                            {
                            ifc.cursor = sys.plant[p]->loc;
                            break;
                            }
                        }
                    while(sys.plant[p] != active_plant);
                    }
            break;

            case '5':
            if(ifc.mode != install_channel_mode && ifc.mode != extend_channel_mode)
                if(ifc.cursor->alfa_duct)
                    {
                    active_duct = ifc.cursor->alfa_duct;
                    do
                        {
                        if(ifc.cursor->beta_flow && ifc.cursor->beta_duct == active_duct)
                            ifc.cursor = ifc.cursor->beta_flow;
                        else
                            if(ifc.cursor->alfa_flow)
                                ifc.cursor = ifc.cursor->alfa_flow;
                            else
                                {
                                ifc.cursor = ifc.cursor->alfa_duct->start_loc;
                                break;  /* Necessario per i canali orfani */
                                }
                        }
                    while(!ifc.cursor->num_links);
                    }
            break;

            #if !defined NDEBUG

            case '`':
            usr.money += 100 * INITIAL_MONEY;
            display_user();
            break;

            case '~':
            for(command.loc_arg = &env.map[0][0]; command.loc_arg <= &env.map[MAP_ROWS - 1][MAP_COLS - 1]; command.loc_arg++)
                command.loc_arg->explored = TRUE;
            break;

            #endif

            default:
            for(dir = 0; dir < DIRECTION_NUMBER; dir++)
                if(input_char == dir_key[dir])
                    {
                    if(ifc.mode == install_channel_mode)
                        {
                        command.opcode = cmd_add_chl;
                        command.loc_arg = ifc.cursor;
                        command.dir_arg = dir;
                        ifc.mode = extend_channel_mode;
                        }
                    else
                        if(ifc.mode == extend_channel_mode)
                            {
                            command.opcode = cmd_ext_chl;
                            command.loc_arg = ifc.cursor;
                            command.dir_arg = dir;
                            }
                    ifc.cursor = neighbor(ifc.cursor, dir);
                    break;
                    }
            if(dir == DIRECTION_NUMBER)
                display_message(ask_for_help);
            break;
            }
        }
    while(command.opcode == cmd_nop);

    return command;
    }

int query_for_item(const char **name, const icon_type *icon, const real *price,
    int *storage, int class_number)
    {
    int class;
    char input_char;

    locate(LIST_ROW, 0);
    set_style(standard_style);
    clear_to_eos();
    for(class = 0; class < class_number; class++)
        {
        if(class == 2 * (class / 2))
            locate(LIST_ROW + class / 2, LIST_FIRST_COL);
        else
            locate(LIST_ROW + class / 2, LIST_SECOND_COL);

        if(storage[class])
            con_printf("%3d ", storage[class]);
        else
            write_string("    ");

        con_printf("[%c] %-16s %4.0f Cr",
            toupper(icon[class]), name[class], price[class]);
        }

    input_char = read_char();
    input_char = toupper(input_char);
    for(class = 0; class < class_number && input_char != toupper(icon[class]); class++);

    return class;
    }

void pause()
    {
    display_mode(pause_mode);
    read_char();
    }

boolean receive_interrupt()
    {
    return async_read_char();
    }

void lock_interface()
    {
    display_mode(run_mode);
    }

void display_symbol(symbol *sp)
    {
    char ch;
    int pen;

    sp->updated = FALSE;
    switch(sp->attribute)
        {
        case plain:
        ch = sp->icon;
        pen = cfg.color_pen[sp->color];
        break;

        case idle_device:
        case empty_device:
        ch = tolower(sp->icon);
        pen = reverse_pen(cfg.color_pen[sp->color]);
        break;

        case active_device:
        case full_device:
        ch = toupper(sp->icon);
        pen = reverse_pen(cfg.color_pen[sp->color]);
        break;

        case broken_device:
        ch = tolower(sp->icon);
        pen = cfg.color_pen[sp->color];
        break;

        case broken_channel:
        ch = BREAK_ICON;
        pen = cfg.color_pen[sp->color];
        break;

        case idle_mobile:
        ch = tolower(sp->icon);
        pen = cfg.color_pen[sp->color];
        break;

        case active_mobile:
        ch = toupper(sp->icon);
        pen = cfg.color_pen[sp->color];
        break;

        case gravid_mobile:
        ch = toupper(sp->icon);
        pen = reverse_pen(cfg.color_pen[sp->color]);
        break;

        case hit_mobile:
        ch = BLOW_ICON;
        pen = cfg.color_pen[sp->color];
        break;

        default:
        assert(0);
        break;
        }

    set_pen(pen);
    write_char(ch);
    }

void display_map()
    {
    int i, j;
    resource_type g;

    locate(MAP_ROW, 0);
    for(i = 0; i < MAP_ROWS; i++)
        {
        for(j = 0; j < MAP_COLS; j++)
            display_symbol(&env.map[i][j].sym);

        if(i < LEGEND_ROW + PRIMARY_RT_NUMBER)
            {
            if(i < LEGEND_ROW)
                {
                if(!i)
                    {
                    set_style(standard_style);
                    con_printf("   Resource       UP   DP");
                    }
                }
            else
                {
                g = i - LEGEND_ROW;
                set_pen(reverse_pen(cfg.color_pen[resource_color[g]]));
                write_string("  ");
                set_style(standard_style);
                con_printf(" %-12s  %4.2f  %2.0f",  resource_name[g],
                                                    resource_unit_price[g],
                                                    channel_unit_price[g]);
                }
            }
        write_char('\n');
        }

    set_style(standard_style);
    con_printf("\n\n" BAR);
    }

void display_cell(cell *loc)
    {
    if(loc->sym.updated)
        {
        locate(loc->pos.i, loc->pos.j);
        display_symbol(&loc->sym);
        }
    }

void display_neighbors(cell *loc)
    {
    direction dir;

    display_cell(loc);
    for(dir = 0; dir < DIRECTION_NUMBER; dir++)
        display_cell(neighbor(loc, dir));
    }

void display_factors()
    {
    set_style(standard_style);

    locate(ENV_ROW, ENV_COL);
    con_printf("Year %d, Day %d",
        env.time / BASE_PERIOD + 1, env.time % BASE_PERIOD + 1);
    clear_to_eol();

    locate(ENV_ROW + 1, ENV_COL);
    write_string(season_name[(int)floor(0.5 + env.time / ((real)BASE_PERIOD / SEASONS_NUMBER)) %
                            SEASONS_NUMBER]);
    clear_to_eol();

    locate(ENV_ROW + 3, ENV_COL);
    con_printf("Solar factor:  %s",
                quality[round((QUALITIES_NUMBER - 1) * env.solar_factor)]);
    clear_to_eol();

    locate(ENV_ROW + 4, ENV_COL);
    con_printf("Temperature:   %s",
                quality[round((QUALITIES_NUMBER - 1) * env.temperature_factor)]);
    clear_to_eol();

    locate(ENV_ROW + 5, ENV_COL);
    con_printf("Water quality: %s",
                quality[round((QUALITIES_NUMBER - 1) * env.water_factor)]);
    clear_to_eol();

    locate(ENV_ROW + 6, ENV_COL);
    con_printf("Air quality:   %s",
                quality[round((QUALITIES_NUMBER - 1) * env.air_factor)]);
    clear_to_eol();

    locate(ENV_ROW + 7, ENV_COL);
    con_printf("Lichen growth: %s",
                quality[round((QUALITIES_NUMBER - 1) * env.growth_factor)]);
    clear_to_eol();
    }

void display_user()
    {
    locate(USR_ROW, USR_COL);
    set_style(highlight_style);
    con_printf("Credits: %12.0f", usr.money);
    set_style(standard_style);
    clear_to_eol();

    locate(USR_ROW + 1, USR_COL);
    set_style(highlight_style);
    con_printf("Humans: %13d", usr.humans);
    set_style(standard_style);
    clear_to_eol();

    locate(USR_ROW + 2, USR_COL);
    set_style(highlight_style);
    con_printf("Hybrids: %12d", usr.hybrids);
    set_style(standard_style);
    clear_to_eol();

    locate(USR_ROW + 3, USR_COL);
    set_style(highlight_style);
    con_printf("Pantropic forms: %4d", usr.pantropics);
    set_style(standard_style);
    clear_to_eol();

    locate(USR_ROW + 4, USR_COL);
    set_style(highlight_style);
    con_printf("Robots: %13d", usr.robots);
    set_style(standard_style);
    clear_to_eol();

    locate(USR_ROW + 5, USR_COL);
    set_style(highlight_style);
    con_printf("Score: %14.0f", usr.score);
    set_style(standard_style);
    clear_to_eol();
    }

void display_zoom(cell *loc)
    {
    locate(ZOOM_ROW, 0);
    if(loc && loc->explored)
        {
        if(loc->plant)
            display_device(loc->plant);
        else
            {
            if(loc->alfa_duct)
                {
                display_channel(loc->alfa_duct);
                if(loc->beta_duct)
                    {
                    write_char('\n');
                    clear_to_eol();
                    set_style(highlight_style);
                    con_printf("\n%s duct #%d",
                        resource_name[loc->beta_duct->throughput_type],
                        loc->beta_duct->num + 1);
                    set_style(standard_style);
                    con_printf(" as secondary duct");
                    clear_to_eol();
                    write_string("\n  Press [Enter] to swap");
                    }
                }
            else
                if(loc->turtle)
                    display_mobile(loc->turtle);
                else
                    display_ground_infos(loc);
            }
        }
    clear_to_eos();
    }

void display_ground_infos(cell *loc)
    {
    set_style(halflight_style);
    con_printf("Monitored Area");
    set_style(standard_style);
    clear_to_eol();

    con_printf("\nGround fertility: %s",
        quality[round((QUALITIES_NUMBER - 1) * loc->ground_factor)]);
    clear_to_eol();
    con_printf("\nWater and air processing fitness: %s",
        quality[round((QUALITIES_NUMBER - 1) * (1 - loc->ground_factor))]);
    clear_to_eol();
    }

void display_device(device *plant)
    {
    resource_type g;
    const char *mu;

    /* Se output_type è >= PRIMARY_RT_NUMBER bisogna fare qualcosa di più */
    mu = measure_unit[plant->output_type];

    set_style(highlight_style);
    con_printf("%s #%d", device_name[plant->id.class], plant->id.num + 1);
    set_style(standard_style);
    clear_to_eol();

    if(plant->max_capacity)
        {
        con_printf("\n%s stored: %.2f %s (%.0f%% of capacity: %.2f %s)",
                    resource_name[plant->output_type],
                    plant->current_capacity, mu,
                    100 * plant->current_capacity / plant->max_capacity,
                    plant->max_capacity, mu);
        clear_to_eol();
        }

    con_printf("\nCurrent output: %.2f %s/day (%.0f%% of measured peak: %.2f %s/day)",
                plant->current_output, mu,
                100 * plant->current_output / (plant->eta * plant->max_state),
                plant->eta * plant->max_state, mu);
    clear_to_eol();

    if(plant->demand_driven)
        con_printf("\nAuto target output");
    else
        {
        set_style(control_style);
        con_printf("\nManual output control");
        set_style(standard_style);
        }
    con_printf(": %.2f %s/day (%.0f%% of nominal peak: %.2f %s/day)",
                plant->target_output, mu,
                100 * plant->target_output / plant->max_state,
                plant->max_state, mu);
    clear_to_eol();

    if(plant->broken)
        {
        set_style(failure_style);
        con_printf("\n*** Broken ***");
        set_style(standard_style);
        clear_to_eol();
        }

    for(g = 0; g < PRIMARY_RT_NUMBER; g++)
        if(plant->unit_consume[g])
            {
            con_printf("\nPlant providers:");
            clear_to_eol();
            break;
            }
    while(g < PRIMARY_RT_NUMBER)
        {
        if(plant->unit_consume[g])
            {
            if(plant->input[g])
                con_printf("\n  %s duct #%d", resource_name[g], plant->input[g]->num + 1);
            else
                con_printf("\n  No connection to %s ducts", resource_name[g]);
            clear_to_eol();
            }
        g++;
        }
    }

void display_channel(channel *duct)
    {
    device *provider;
    device_class provider_class;
    int p;
    const char *mu;

    mu = measure_unit[duct->throughput_type];

    set_style(highlight_style);
    con_printf("%s duct #%d", resource_name[duct->throughput_type], duct->num + 1);
    set_style(standard_style);
    clear_to_eol();

    con_printf("\nCurrent throughput: %.2f %s/day", duct->current_throughput, mu);
    clear_to_eol();
    con_printf("\nTarget throughput: %.2f %s/day, Demand: %.2f %s/day",
        duct->target_throughput, mu, duct->demanded_throughput, mu);
    clear_to_eol();
    con_printf("\nOverall length: %d", duct->length);
    clear_to_eol();

    if(duct->break_loc)
        {
        set_style(failure_style);
        con_printf("\n*** Broken ***");
        set_style(standard_style);
        clear_to_eol();
        }

    if(duct->provider[0])
        {
        con_printf("\nDuct providers:");

        provider_class = DEVICE_CLASS_NUMBER;
        for(p = 0; p < MAX_PROVIDERS_NUMBER; p++)
            {
            provider = duct->provider[p];
            if(!provider)
                break;

            if(provider_class != provider->id.class)
                {
                provider_class = provider->id.class;
                clear_to_eol();
                con_printf("\n  %s #%d", device_name[provider_class], provider->id.num + 1);
                }
            else
                con_printf(", %d", provider->id.num + 1);
            }
        clear_to_eol();
        }
    }

void display_mobile(mobile *turtle)
    {
    program_tag tag;

    set_style(highlight_style);
    write_string(mobile_name[turtle->class]);
    set_style(standard_style);
    clear_to_eol();

    if(turtle->class < PRIMARY_MC_NUMBER)
        {
        if(turtle->remote_control)
            con_printf("\nRobot remote controlled by computer");
        else
            con_printf("\nRobot controlled by embedded cpu");
        clear_to_eol();

        if(!turtle->active)
            {
            set_style(failure_style);
            con_printf("\n*** Shutted down ***");
            set_style(standard_style);
            clear_to_eol();
            }

        con_printf("\nCondition: %s", quality[round((QUALITIES_NUMBER - 1) * turtle->eta)]);
        clear_to_eol();

        if(turtle->remote_control)
            {
            con_printf("\nAvailable programs:");
            clear_to_eol();
            for(tag = 0; tag < PRIMARY_PT_NUMBER; tag++)
                {
                con_printf("\n%s%s",
                            (tag == turtle->mcp.tag)? "=>" : "  ", program_tag_name[tag]);
                clear_to_eol();
                }
            }
        }
    }

void display_frame()
    {
    display_factors();
    display_user();
    display_zoom(ifc.cursor);
    }

void display_mode(input_mode mode)
    {
    locate(MODE_ROW, MODE_COL);
    switch(mode)
        {
        case start_mode:
        set_style(install_style);
        con_printf("Planet scan mode");
        break;

        case control_mode:
        set_style(control_style);
        con_printf("Control mode");
        break;

        case install_channel_mode:
        set_style(install_style);
        con_printf("Create duct mode");
        break;

        case extend_channel_mode:
        set_style(extend_style);
        con_printf("Extend duct mode");
        break;

        case trade_mode:
        set_style(trade_style);
        con_printf("Trade mode");
        break;

        case watch_mode:
        set_style(failure_style);
        con_printf("Watch mode");
        break;

        case run_mode:
        set_style(standard_style);
        con_printf("Simulation running...");
        break;

        case pause_mode:
        set_style(standard_style);
        con_printf("Any key to continue...");
        break;

        default:
        assert(0);
        break;
        }
    set_style(standard_style);
    clear_to_eol();
    }

void display_message(message_class message)
    {
    locate(MESSAGE_ROW, MESSAGE_COL);
    set_style(highlight_style);
    write_string(message_text[message]);
    set_style(standard_style);
    clear_to_eol();

    if(message != blank_message)
        {
        ifc.history.last = (ifc.history.last + 1) % HISTORY_LEN;
        ifc.history.message[ifc.history.last] = message;
        ifc.history.time[ifc.history.last] = env.time;
        ifc.history.clear_message = TRUE;
        }
    else
        ifc.history.clear_message = FALSE;
    }

void display_history()
    {
    int k;

    locate(HISTORY_ROW, 0);
    set_style(standard_style);
    clear_to_eos();

    k = ifc.history.last;
    do
        {
        k = (k + 1) % HISTORY_LEN;
        if(ifc.history.message[k] != blank_message)
            con_printf("%2d.%03d> %s\n",
            ifc.history.time[k] / BASE_PERIOD + 1, ifc.history.time[k] % BASE_PERIOD + 1,
            message_text[ifc.history.message[k]]);

        }
    while(k != ifc.history.last);
    }

void display_stats()
    {
    real avg_plant_eta, avg_turtle_eta;
    real plants_value, ducts_value, turtles_value, stored_value, tot_value;
    int tot_length;
    int p, d, t;
    device *plant;
    channel *duct;
    mobile *turtle;

    avg_plant_eta = 0;
    plants_value = 0;
    stored_value = 0;
    if(sys.tot_plants)
        {
        for(p = 0; p < sys.tot_plants; p++)
            {
            plant = sys.plant[p];
            avg_plant_eta += plant->eta;
            plants_value += plant->eta * device_price[plant->id.class];
            if(plant->current_capacity && plant->output_type < PRIMARY_RT_NUMBER)
                stored_value += plant->current_capacity * resource_unit_price[plant->output_type];
            }
        avg_plant_eta /= sys.tot_plants;
        }

    tot_length = 0;
    ducts_value = 0;
    for(d = 0; d < sys.tot_ducts; d++)
        {
        duct = sys.duct[d];
        tot_length += duct->length;
        ducts_value += duct->length * channel_unit_price[duct->throughput_type];
        }

    avg_turtle_eta = 0;
    turtles_value = 0;
    if(usr.robots)
        {
        for(t = 0; t < sys.tot_turtles; t++)
            {
            turtle = sys.turtle[t];
            if(turtle && turtle->class < PRIMARY_MC_NUMBER)
                {
                avg_turtle_eta += turtle->eta;
                turtles_value += turtle->eta * mobile_price[turtle->class];
                }
            }
        avg_turtle_eta /= usr.robots;
        }

    tot_value = plants_value + ducts_value + turtles_value + stored_value;

    locate(STATS_ROW, 0);
    set_style(standard_style);
    clear_to_eos();
    con_printf( "Colony estimated value:\n"
                "\n"
                "  %6.0f Cr in %d plants, average efficiency %.0f%%\n"
                "  %6.0f Cr in %d ducts, overall length %d\n"
                "  %6.0f Cr in %d robots, average efficiency %.0f%%\n"
                "  %6.0f Cr in stored resources\n"
                "  ------\n"
                "  %6.0f Cr",
                plants_value, sys.tot_plants, 100 * avg_plant_eta,
                ducts_value, sys.tot_ducts, tot_length,
                turtles_value, usr.robots, 100 * avg_turtle_eta,
                stored_value,
                tot_value);
    }

/* *INDENT-OFF* */
void display_help()
    {
    locate(HELP_ROW, 0);
    set_style(standard_style);
    clear_to_eos();
    con_printf(
    "  7 8 9                                              0    Forward to next day\n"
    "  4   6  Move cursor                                Tab   Forward till stop\n"
    "  1 2 3                                              >    Forward to next year\n"
    "    5    Follow duct to link point                  <--   Show message history\n"
    "    .    Jump on control or break point              ?    Show help\n"
    "  Enter  Switch plant control/swap ducts             $    Show statistics\n"
    "  + - *  Set output/trade resource/select program    R    Redraw screen\n"
    "    [    Buy/install plant                           D    Dump map\n"
    "    ]    Buy/install robot                           L    Load game\n"
    "    /    Create/extend duct                          S    Save game\n"
    "    X    Destroy plant/duct                          Q    Quit game\n"
    "                                                    Esc   Abort game");
    }
/* *INDENT-ON* */

void set_style(style_type style)
    {
    set_pen(cfg.style_pen[style]);
    }

void redraw_screen()
    {
    begin_redraw();
    set_style(standard_style);
    clear_screen();
    display_map();
    if(ifc.mode != start_mode)
        display_frame();
    end_redraw();
    }

