
#include "global.h"
#include "coredefs.h"
#include "params.h"
#include "texts.h"
#include "symbols.h"
#include "elan.h"
#include "interf.h"
#include "files.h"

environment env;
systems sys;
user usr;

int main(int argc, char *argv[])
    {
    event news;
    cell *active_loc, *backup_loc;
    user_command command;
    channel *active_duct;
    device_class dev_class;
    mobile_class mob_class;
    resource_type throughput_type;
    int p, d;

    open_io();
    srand((unsigned int)time(NULL));

    init_systems();
    init_user();
    init_environment();

    display_map();
    display_message(place_habitat);

    news = no_news;
    for(;;)
        {
        command = query_for_command(news);

        news = no_news;
        switch(command.opcode)
            {
            case cmd_day:
            news = next_day(TRUE);
            break;

            case cmd_forward:
            lock_interface();
            do
                news = next_day(TRUE);
            while(news == time_passed && !receive_interrupt());
            unlock_interface();
            break;

            case cmd_year:
            news = next_year(FALSE);
            break;

            case cmd_swap_chl:
            swap_channels(command.loc_arg);
            display_cell(command.loc_arg);
            break;

            case cmd_add_dev:
            if(has_around_plants(command.loc_arg))
                {
                display_message(neighbor_plant);
                break;
                }
            display_message(select_item_to_install);
            dev_class = query_for_item(device_name, device_icon, device_price,
                        usr.device_storage, DEVICE_CLASS_NUMBER);
            if(dev_class == DEVICE_CLASS_NUMBER)
                {
                display_message(blank_message);
                break;
                }
            if(!usr.device_storage[dev_class])
                {
                display_message(item_not_available);
                break;
                }
            if(install_device(dev_class, command.loc_arg))
                {
                usr.device_storage[dev_class]--;

                display_neighbors(command.loc_arg);
                display_message(blank_message);
                news = next_day(TRUE);
                }
            break;

            case cmd_del_dev:
            remove_device(command.loc_arg->plant);
            display_neighbors(command.loc_arg);
            break;

            case cmd_del_chl:
            active_duct = command.loc_arg->alfa_duct;

            active_loc = backup_loc = active_duct->start_loc;
            while(active_loc)
                {
                if(active_loc->alfa_duct == active_duct)
                    {
                    backup_loc = active_loc->alfa_flow;

                    active_loc->alfa_duct = active_loc->beta_duct;
                    active_loc->alfa_flow = active_loc->beta_flow;
                    active_loc->beta_duct = NULL;
                    active_loc->beta_flow = NULL;
                    }
                else
                    {
                    backup_loc = active_loc->beta_flow;

                    active_loc->beta_duct = NULL;
                    active_loc->beta_flow = NULL;
                    }

                active_loc->break_flow = NULL;
                active_loc->num_links = 0;

                if(active_loc->alfa_duct)
                    {
                    set_icon(&active_loc->sym, CORNER_ICON);
                    set_color(&active_loc->sym,
                        resource_color[active_loc->alfa_duct->throughput_type]);
                    }
                else
                    clear_cell(active_loc);

                display_cell(active_loc);

                active_loc = backup_loc;
                }

            for(p = 0; p < sys.tot_plants; p++)
                {
                if(sys.plant[p]->input[active_duct->throughput_type] == active_duct)
                    {
                    sys.plant[p]->input[active_duct->throughput_type] = NULL;
                    link_neighbors(sys.plant[p]->loc);
                    display_neighbors(sys.plant[p]->loc);
                    }
                }

            for(d = 0; d < sys.tot_ducts; d++)
                {
                if(sys.duct[d] == active_duct)
                    {
                    sys.tot_ducts--;
                    sys.duct[d] = sys.duct[sys.tot_ducts];
                    break;
                    }
                }

            free(active_duct);
            break;

            case cmd_add_mob:
            if(!command.loc_arg->explored)
                {
                display_message(square_not_explored);
                break;
                }
            display_message(select_item_to_install);
            mob_class = query_for_item(mobile_name, mobile_icon, mobile_price,
                                        usr.mobile_storage, PRIMARY_MC_NUMBER);
            if(mob_class == PRIMARY_MC_NUMBER)
                {
                display_message(blank_message);
                break;
                }
            if(!usr.mobile_storage[mob_class])
                {
                display_message(item_not_available);
                break;
                }
            if(install_mobile(mob_class, command.loc_arg))
                {
                usr.mobile_storage[mob_class]--;

                display_cell(command.loc_arg);
                display_message(blank_message);
                news = next_day(TRUE);
                }
            break;

            case cmd_buy_dev:
            purchase_items(device_name, device_icon, device_price,
                            usr.device_storage, DEVICE_CLASS_NUMBER);
            break;

            case cmd_buy_mob:
            purchase_items(mobile_name, mobile_icon, mobile_price,
                            usr.mobile_storage, PRIMARY_MC_NUMBER);
            break;

            case cmd_trade_dev:
            trade_resource(command.loc_arg->plant,
                command.loc_arg->plant->max_capacity * max(0, min(1, command.num_arg)));
            display_cell(command.loc_arg);
            display_user();
            break;

            case cmd_switch_dev:
            command.loc_arg->plant->demand_driven = !command.loc_arg->plant->demand_driven;
            break;

            case cmd_set_dev:
            if(!command.loc_arg->plant->demand_driven)
                command.loc_arg->plant->target_output =
                    command.loc_arg->plant->max_state * max(0, min(1, command.num_arg));
            break;

            case cmd_set_mob:
            if(command.loc_arg->turtle->remote_control)
                upload_program(((int)floor(command.num_arg) + PRIMARY_PT_NUMBER) %
                                PRIMARY_PT_NUMBER, command.loc_arg->turtle);
            break;

            case cmd_reset:
            init_environment();
            display_map();
            display_message(place_habitat);
            break;

            case cmd_start:
            start_game(command.loc_arg);
            news = shuttle_arrival;
            break;

            case cmd_load:
            load_game();
            news = time_passed;
            break;

            case cmd_save:
            save_game();
            break;

            case cmd_dump:
            if(dump_map() != ok)
                display_message(save_error);
            break;

            case cmd_quit:
            end_game();
            pause();
            clean_exit(0);
            break;

            case cmd_esc:
            clean_exit(0);
            break;

            case cmd_add_chl:
            throughput_type = command.loc_arg->plant->output_type;
            if(throughput_type >= PRIMARY_RT_NUMBER)
                {
                news = extension_failure;
                display_message(resource_not_exportable);
                break;
                }
            if(channel_unit_price[throughput_type] > usr.money)
                {
                news = extension_failure;
                display_message(insuff_money_to_build_channel);
                break;
                }
            active_duct = install_channel(command.loc_arg->plant, command.dir_arg);
            if(!active_duct)
                {
                news = extension_failure;
                break;
                }
            usr.money -= channel_unit_price[throughput_type];

            display_neighbors(active_duct->end_loc);
            news = next_day(TRUE);
            break;

            case cmd_ext_chl:
            active_duct = command.loc_arg->alfa_duct;
            throughput_type = active_duct->throughput_type;
            if(channel_unit_price[throughput_type] > usr.money)
                {
                news = extension_failure;
                display_message(insuff_money_to_build_channel);
                break;
                }
            if(!extend_channel(active_duct, command.loc_arg, command.dir_arg))
                {
                news = extension_failure;
                break;
                }
            usr.money -= channel_unit_price[throughput_type];

            display_neighbors(active_duct->end_loc);
            news = next_day(TRUE);
            break;

            default:
            assert(0);
            break;
            }
        }
    }

void clean_exit(int rv)
    {
    int p;

    for(p = 0; p < sys.tot_plants; p++)
        free(sys.plant[p]);

    for(p = 0; p < sys.tot_ducts; p++)
        free(sys.duct[p]);

    for(p = 0; p < sys.tot_turtles; p++)
        free(sys.turtle[p]);

    close_io();

    exit(rv);
    }

void purchase_items(const char **name, const icon_type *icon, const real *price,
    int *storage, int class_number)
    {
    int class;

    display_message(select_item_to_buy);
    for(;;)
        {
        class = query_for_item(name, icon, price, storage, class_number);

        if(class == class_number)
            {
            display_message(blank_message);
            break;
            }
        if(price[class] > usr.money)
            {
            display_message(insuff_money_to_buy_item);
            break;
            }

        usr.money -= price[class];
        storage[class]++;

        display_user();
        }
    }

void explore_neighbors(cell *loc)
    {
    cell *near_loc;
    direction dir;

    for(dir = 0; dir < DIRECTION_NUMBER; dir++)
        {
        near_loc = neighbor(loc, dir);
        if(!near_loc->explored)         /* Impianti e canali non vi rientrano */
            {
            near_loc->explored = TRUE;
            if(near_loc->turtle)
                set_mobile_icon(near_loc->turtle);
            else
                clear_cell(near_loc);
            }
        }
    }

void link_neighbors(cell *loc)
    {
    cell *near_loc, *link_loc;
    direction dir;

    for(dir = 0; dir < DIRECTION_NUMBER; dir++)
        {
        near_loc = neighbor(loc, dir);
        link_loc = NULL;

        if(loc->plant)
            {
            if(near_loc->alfa_duct && !near_loc->beta_duct)
                {
                if(link_device(loc->plant, near_loc->alfa_duct))        /* Non mettere || */
                    link_loc = near_loc;
                if(link_channel(near_loc->alfa_duct, loc->plant))
                    link_loc = near_loc;
                }
            }
        else
            if(loc->alfa_duct && !loc->beta_duct)
                {
                if(near_loc->plant)
                    {
                    if(link_device(near_loc->plant, loc->alfa_duct))        /* Non mettere || */
                        link_loc = loc;
                    if(link_channel(loc->alfa_duct, near_loc->plant))
                        link_loc = loc;
                    }
                }
            else
                break;

        if(link_loc)
            {
            link_loc->num_links++;
            set_icon(&link_loc->sym, LINK_ICON(link_loc->num_links));
            }
        }
    }

real trade_resource(device *plant, real requested_capacity)
    {
    real traded_capacity;
    real traded_money;

    assert(requested_capacity >= 0 && requested_capacity <= plant->max_capacity);

    if(plant->output_type >= PRIMARY_RT_NUMBER || plant->broken)
        return 0;

    traded_capacity = requested_capacity - plant->current_capacity;
    traded_money = resource_unit_price[plant->output_type] * traded_capacity;
    if(usr.money >= traded_money)
        {
        usr.money -= traded_money;
        plant->current_capacity += traded_capacity;
        }
    else
        {
        traded_capacity = usr.money / resource_unit_price[plant->output_type];
        plant->current_capacity += traded_capacity;
        usr.money = 0;
        }

    update_device_icon(plant);

    return traded_capacity;
    }

event next_day(boolean trace)
    {
    event news;

    update_systems(trace);

    if(usr.colony_dead)
        news = time_passed;
    else
        {
        if(usr.humans || usr.hybrids)
            {
            switch(env.time % BASE_PERIOD)
                {
                case 0:
                if(place_around_providers(1, space_shuttle, human_life_support))
                    {
                    usr.shuttle_landed = TRUE;
                    news = shuttle_arrival;
                    display_message(shuttle_arrived);
                    }
                else
                    news = time_passed;
                break;

                case SHUTTLE_STAY:
                if(usr.shuttle_landed)
                    {
                    usr.shuttle_landed = FALSE;
                    news = shuttle_departure;
                    display_message(shuttle_departed);
                    }
                else
                    news = time_passed;
                break;

                default:
                news = time_passed;
                break;
                }
            }
        else
            {
            usr.colony_dead = TRUE;
            news = colony_end;
            display_message(colony_terminated);
            }
        }

    if(trace)
        display_frame();

    return news;
    }

event next_year(boolean trace)
    {
    event news;

    lock_interface();
    do
        news = next_day(trace);
    while(news == time_passed && env.time % BASE_PERIOD);
    if(!trace)
        {
        display_map();
        display_frame();
        }
    unlock_interface();

    return news;
    }

void update_systems(boolean trace)
    {
    resource_type g;
    channel *duct;
    device *plant;
    mobile *turtle;
    int p, d, t;
    cell *loc, *near_loc;
    real gamma, delta;
    int phi, psi;

    update_environment();
    if(random(1) < GIZMO_FACTOR)
        {
        loc = &env.map[(int)floor(random(MAP_ROWS))][(int)floor(random(MAP_COLS))];
        if(!loc->explored && !loc->turtle && random(1) < loc->ground_factor)
            install_mobile(plasma_gizmo, loc);
        }

    for(d = 0; d < sys.tot_ducts; d++)
        {
        duct = sys.duct[d];
        duct->current_throughput = 0;
        duct->target_throughput = 0;
        duct->demanded_throughput = 0;

        loc = duct->break_loc;
        if(loc &&
            random(pow((real)(sys.tot_ducts * duct->length), 0.5)) <
            random(usr.humans * REPAIR_FACTOR))
            {
            if(loc->break_flow == loc)
                {
                duct->break_loc = NULL;
                display_message(channel_repaired);
                }
            else
                {
                loc = loc->break_flow;
                duct->break_loc->break_flow = loc->break_flow;
                }
            loc->break_flow = NULL;

            set_attribute(&loc->sym, plain);
            if(trace)
                display_cell(loc);
            }
        }

    sys.demand[human_life_support - SECONDARY_RT_BASE] += usr.humans + usr.human_growing;
    sys.demand[hybrid_life_support - SECONDARY_RT_BASE] += usr.hybrids + usr.hybrid_growing +
                                                        usr.pantropic_growing;
    sys.demand[morphing_power - SECONDARY_RT_BASE] += usr.humans;
    /* += in caso qualche mobile ne avesse fatto richiesta */

    for(g = 0; g < SECONDARY_RT_NUMBER; g++)
        sys.offer[g] = 0;

    for(p = 0; p < sys.tot_plants; p++)
        {
        plant = sys.plant[p];
        loc = plant->loc;

        if(plant->broken &&
            random(sys.tot_plants) <
            random(usr.humans * ((plant->output_type == human_life_support)?
                                        CRITICAL_REPAIR_FACTOR : REPAIR_FACTOR)))
            {
            plant->broken = FALSE;
            display_message(system_repaired);
            }

        if(plant->output_type < PRIMARY_RT_NUMBER)
            update_device(plant);
        else
            {
            g = plant->output_type - SECONDARY_RT_BASE;

            if(sys.demand[g] <= plant->available_output)
                {
                plant->surplus_output -= sys.demand[g];
                sys.demand[g] = 0;
                }
            else
                {
                sys.demand[g] -= plant->available_output;
                plant->surplus_output = -plant->max_state;  /* Tempo di salita minimo */
                }
            plant->available_output = 0;        /* Risorsa secondaria non recuperabile */

            update_device(plant);

            sys.offer[g] += plant->available_output;
            }

        if(trace)
            display_cell(loc);
        }

    for(g = 0; g < SECONDARY_RT_NUMBER; g++)
        sys.demand[g] = 0;

    if(sys.sort_turtles)
        {
        sys.sort_turtles = FALSE;
        qsort(sys.turtle, sys.tot_turtles, sizeof(*sys.turtle),
                (int (*)(const void *, const void *))mobile_priority);
        while(!sys.turtle[sys.tot_turtles - 1])
            sys.tot_turtles--;
        }

    for(t = 0; t < sys.tot_turtles; t++)
        {
        turtle = sys.turtle[t];
        loc = turtle->loc;

        if(turtle->time_to_remove)
            {
            turtle->time_to_remove--;
            if(!turtle->time_to_remove)
                {
                remove_mobile(turtle);
                turtle = NULL;
                }
            }

        if(turtle)
            {
            if(turtle->remote_control)
                {
                sys.demand[cpu_power - SECONDARY_RT_BASE]++;    /* Per il ciclo successivo */
                if(sys.offer[cpu_power - SECONDARY_RT_BASE] >= 1)
                    {
                    sys.offer[cpu_power - SECONDARY_RT_BASE]--;
                    turtle->active = TRUE;
                    }
                else
                    turtle->active = FALSE;
                }

            near_loc = update_mobile(turtle);
            if(trace && near_loc)
                display_cell(near_loc);     /* Può essere != turtle->loc */
            }

        if(trace)
            display_cell(loc);
        }

    delta = delta_people(usr.humans + usr.human_growing,
                        sys.offer[human_life_support - SECONDARY_RT_BASE]);
    if(delta < 0)
        {
        if(usr.humans)
            display_message(human_support_critical);

        usr.humans = floor(usr.humans + usr.human_growing + delta);
        usr.human_growing = 0;
        }
    else
        {
        usr.human_growing += delta;
        phi = floor(usr.human_growing);
        if(phi)
            {
            usr.human_growing -= phi;
            usr.humans += phi;

            display_message(human_birth);
            }
        }

    delta = delta_people(usr.hybrids + usr.pantropic_growing,
                        sys.offer[hybrid_life_support - SECONDARY_RT_BASE]);
    if(delta < 0)
        {
        if(usr.hybrids)
            display_message(hybrid_support_critical);

        usr.hybrids = floor(usr.hybrids + usr.pantropic_growing + delta);
        usr.pantropic_growing = 0;
        }
    else
        {
        usr.pantropic_growing += delta;
        phi = floor(usr.pantropic_growing);
        if(phi)
            {
            psi = MUTATION_RATIO * phi;
            if(random(1) < MUTATION_RATIO)
                psi++;

            phi = place_around_providers(phi - psi, pantropic_form, hybrid_life_support);
            psi = place_around_providers(psi, mutant_form, hybrid_life_support);

            if(phi || psi)
                {
                usr.pantropic_growing -= phi + psi;

                if(phi >= psi)
                    display_message(pantropic_birth);
                else
                    display_message(mutant_birth);
                }
            }
        }

    usr.hybrid_growing = min(usr.humans,
                            usr.hybrid_growing + sys.offer[morphing_power - SECONDARY_RT_BASE]);
    phi = floor(usr.hybrid_growing);
    if(phi)
        {
        gamma = max(0, sys.offer[hybrid_life_support - SECONDARY_RT_BASE] -
                            (usr.hybrids + usr.pantropic_growing));
        if(phi <= gamma)
            {
            usr.hybrid_growing -= phi;
            usr.humans -= phi;
            usr.hybrids += phi;

            display_message(hybrid_morphed);
            }
        else
            if(usr.hybrid_growing > HYBRID_SUPPORT_MARGIN * gamma)
                usr.hybrid_growing = HYBRID_SUPPORT_MARGIN * gamma;
        }

    usr.score = MONEY_VALUE * usr.money + HUMANS_VALUE * usr.humans +
                HYBRIDS_VALUE * usr.hybrids + PANTROPICS_VALUE * usr.pantropics;
    }

real delta_people(real people, real life_support)
    {
    real delta;

    if(life_support)
        {
        delta = LIFE_KAPPA * people * (1 - people / life_support);
        if(delta < 0)
            delta = max(delta, -people);
        else
            {
            if(people >= 2)
                delta *= BIRTH_RATIO;
            else
                delta = 0;
            }
        }
    else
        delta = -people;

    delta = random(random(delta));

    return delta;
    }

channel *init_channel(device *source_plant, int num, direction dir)
    {
    channel *duct;
    cell *loc;

    assert(source_plant->output_type < PRIMARY_RT_NUMBER);

    loc = neighbor(source_plant->loc, dir);

    if(loc->plant || loc->alfa_duct || loc->turtle)
        {
        display_message(square_not_empty);
        return NULL;    /* Si deve partire a livello 0 */
        }

    duct = calloc(1, sizeof(*duct));
    assert(duct != NULL);

    duct->throughput_type = source_plant->output_type;
    duct->num = num;
    duct->start_loc = duct->end_loc = loc;
    duct->break_loc = NULL;
    duct->length = 1;
    duct->current_throughput = 0;
    duct->target_throughput = 0;
    duct->demanded_throughput = 0;

    loc->alfa_duct = duct;
    loc->explored = TRUE;
    set_icon(&loc->sym, channel_icon[dir]);
    set_color(&loc->sym, resource_color[source_plant->output_type]);

    return duct;
    }

channel *install_channel(device *source_plant, direction dir)
    {
    channel *duct;

    if(sys.tot_ducts == MAX_CHANNEL_NUMBER)
        {
        display_message(too_many_channels);
        return NULL;
        }

    duct = init_channel(source_plant, sys.num_ducts[source_plant->output_type], dir);
    if(duct)
        {
        sys.duct[sys.tot_ducts] = duct;
        sys.tot_ducts++;
        sys.num_ducts[source_plant->output_type]++;

        explore_neighbors(duct->end_loc);
        link_neighbors(duct->end_loc);
        }

    return duct;
    }

cell *extend_channel(channel *duct, cell *loc, direction dir)
    {
    cell *new_loc;

    new_loc = neighbor(loc, dir);

    if(new_loc->plant || new_loc->beta_duct || new_loc->turtle ||
        new_loc->break_flow || new_loc->num_links || new_loc->alfa_duct == duct)
        {
        display_message(impossible_to_cross_square);
        return NULL;
        }

    if(duct->end_loc->alfa_duct == duct)
        duct->end_loc->alfa_flow = new_loc;
    else
        duct->end_loc->beta_flow = new_loc;

    duct->end_loc = new_loc;
    duct->length++;

    if(!new_loc->alfa_duct)
        {
        new_loc->alfa_duct = duct;
        new_loc->explored = TRUE;

        set_icon(&new_loc->sym, channel_icon[dir]);
        set_color(&new_loc->sym, resource_color[duct->throughput_type]);

        explore_neighbors(new_loc);
        link_neighbors(new_loc);
        }
    else
        {
        new_loc->beta_duct = new_loc->alfa_duct;
        new_loc->beta_flow = new_loc->alfa_flow;
        new_loc->alfa_duct = duct;
        new_loc->alfa_flow = NULL;

        set_icon(&new_loc->sym, CROSS_ICON);
        set_color(&new_loc->sym, resource_color[duct->throughput_type]);
        }

    if(loc->sym.icon != channel_icon[dir] && !loc->beta_duct && !loc->num_links)
        set_icon(&loc->sym, CORNER_ICON);

    return new_loc;
    }

void swap_channels(cell *loc)
    {
    channel *backup_duct;
    cell *backup_loc;

    backup_duct = loc->beta_duct;
    backup_loc = loc->beta_flow;
    loc->beta_duct = loc->alfa_duct;
    loc->beta_flow = loc->alfa_flow;
    loc->alfa_duct = backup_duct;
    loc->alfa_flow = backup_loc;

    set_color(&loc->sym, resource_color[backup_duct->throughput_type]);
    }

boolean link_channel(channel *duct, device *plant)
    {
    int p;

    if(duct->throughput_type == plant->output_type)
        {
        for(p = 0; p < MAX_PROVIDERS_NUMBER; p++)
            {
            if(duct->provider[p] == plant)
                break;

            if(!duct->provider[p])
                {
                duct->provider[p] = plant;
                qsort(duct->provider, p + 1, sizeof(*duct->provider),
                        (int (*)(const void *, const void *))device_priority);
                return TRUE;
                }
            }
        }
    return FALSE;
    }

int device_priority(const device **plant_1, const device **plant_2)
    {
    if(!*plant_1)
        return (*plant_2)? 1 : 0;
    else
        if(!*plant_2)
            return -1;
        else
            if((*plant_1)->id.class != (*plant_2)->id.class)
                return (*plant_1)->id.class - (*plant_2)->id.class;
            else
                return (*plant_1)->id.num - (*plant_2)->id.num;
    }

int mobile_priority(const mobile **turtle_1, const mobile **turtle_2)
    {
    if(!*turtle_1)
        return (*turtle_2)? 1 : 0;
    else
        if(!*turtle_2)
            return -1;
        else
            return (*turtle_1)->class - (*turtle_2)->class;
    }

boolean link_device(device *plant, channel *duct)
    {
    if(plant->unit_consume[duct->throughput_type] && !plant->input[duct->throughput_type])
        {
        plant->input[duct->throughput_type] = duct;
        return TRUE;
        }
    else
        return FALSE;
    }

void init_systems()
    {
    sys.tot_plants = 0;
    sys.tot_ducts = 0;
    sys.tot_turtles = 0;
    sys.sort_turtles = FALSE;
    }

void init_user()
    {
    device_class dev_class;
    mobile_class mob_class;

    usr.money = INITIAL_MONEY;
    usr.humans = HABITAT_DEFAULT_PEOPLE;
    usr.hybrids = 0;
    usr.pantropics = 0;
    usr.human_growing = 0;
    usr.hybrid_growing = 0;
    usr.pantropic_growing = 0;
    usr.robots = 0;
    usr.hostiles = 0;
    usr.score = 0;
    for(dev_class = 0; dev_class < DEVICE_CLASS_NUMBER; dev_class++)
        usr.device_storage[dev_class] = 0;
    for(mob_class = 0; mob_class < PRIMARY_MC_NUMBER; mob_class++)
        usr.device_storage[mob_class] = 0;
    usr.shuttle_landed = TRUE;
    usr.colony_dead = FALSE;
    }

void init_environment()
    {
    int i, j, k;
    cell *loc, *loc_2;

    for(i = 0; i < MAP_ROWS; i++)
        for(j = 0; j < MAP_COLS; j++)
            {
            loc = &env.map[i][j];   /* Trama di base, necessaria per creare il ground */

            loc->pos.i = i;
            loc->pos.j = j;
            loc->plant = NULL;
            loc->beta_duct = loc->alfa_duct = NULL;
            loc->beta_flow = loc->alfa_flow = NULL;
            loc->break_flow = NULL;
            loc->num_links = 0;
            loc->ground_factor = AVERAGE_GROUND_FACTOR;
            loc->explored = FALSE;
            }

    loc = &env.map[0][0];
    loc_2 = &env.map[MAP_ROWS / 2][MAP_COLS / 2];
    for(k = 0; k < GROUND_AREA; k++)
        {
        loc = neighbor(loc, (direction)floor(random(DIRECTION_NUMBER)));
        loc_2 = neighbor(loc_2, (direction)floor(random(DIRECTION_NUMBER)));
        loc->ground_factor = min(1, loc->ground_factor + DELTA_GROUND_FACTOR);
        loc_2->ground_factor = max(0, loc_2->ground_factor - DELTA_GROUND_FACTOR);
        }

    for(i = 0; i < MAP_ROWS; i++)
        for(j = 0; j < MAP_COLS; j++)
            clear_cell(&env.map[i][j]);

    env.time = 0;
    env.solar_factor = env.temperature_factor =
        env.air_factor = env.water_factor = env.growth_factor = 0.5;
    update_environment();
    env.time = 0;
    }

void clear_cell(cell *loc)
    {
    if(loc->explored)
        {
        set_icon(&loc->sym,
            explored_ground_icon[round(loc->ground_factor * (GROUND_ICON_NUMBER - 1))]);
        set_color(&loc->sym, explored_square_color);
        }
    else
        {
        set_icon(&loc->sym,
            unexplored_ground_icon[round(loc->ground_factor * (GROUND_ICON_NUMBER - 1))]);
        set_color(&loc->sym, unexplored_square_color);
        }
    set_attribute(&loc->sym, plain);
    }

cell *neighbor(cell *loc, direction dir)
    {
    position pos;

    pos = loc->pos;

    switch(dir)
        {
        case N:
        pos.i--;
        break;

        case S:
        pos.i++;
        break;

        case W:
        pos.j--;
        break;

        case E:
        pos.j++;
        break;

        case NW:
        pos.i--;
        pos.j--;
        break;

        case SW:
        pos.i++;
        pos.j--;
        break;

        case NE:
        pos.i--;
        pos.j++;
        break;

        case SE:
        pos.i++;
        pos.j++;
        break;

        default:
        assert(0);
        break;
        };

    if(pos.i < 0)
        pos.i = MAP_ROWS - 1;
    else
        {
        if(pos.i >= MAP_ROWS)
            pos.i = 0;
        }

    if(pos.j < 0)
        pos.j = MAP_COLS - 1;
    else
        {
        if(pos.j >= MAP_COLS)
            pos.j = 0;
        }

    return &env.map[pos.i][pos.j];
    }

device *init_device(device_id plant_id, cell *loc)
    {
    device *plant;

    plant = calloc(1, sizeof(*plant));
    assert(plant != NULL);

    plant->id = plant_id;
    plant->loc = loc;
    plant->demand_driven = TRUE;
    plant->zeta_f = zeta_1;
    plant->tau = 0;
    plant->broken = FALSE;

    /* Eta: 0.95 per gli impianti di trasformazione, 0.98 per quelli di stoccaggio */
    switch(plant_id.class)
        {
        case solar_cell:
        plant->output_type = electricity;
        plant->eta = 0.95;
        plant->max_state = 600;
        plant->kappa = 0.70;
        plant->zeta_f = zeta_solar;
        plant->sigma = 0.001;
        break;

        case nuclear_reactor:
        plant->output_type = electricity;
        plant->eta = 0.95;
        plant->max_state = 5000;
        plant->kappa = 0.50;
        plant->unit_consume[water] = 0.05;
        plant->sigma = 0.001;
        break;

        case battery:
        plant->output_type = electricity;
        plant->eta = 0.98;
        plant->max_state = 800;
        plant->kappa = 0.90;
        plant->max_capacity = 12000;
        plant->unit_consume[electricity] = 1;
        plant->sigma = 0.0002;
        break;

        case heating_system:
        plant->output_type = heat;
        plant->eta = 0.95;
        plant->max_state = 220;
        plant->kappa = 0.25;
        plant->unit_consume[electricity] = 2;
        plant->zeta_f = zeta_temperature;
        plant->sigma = 0.001;
        break;

        case water_processor:
        plant->output_type = water;
        plant->eta = 0.95;
        plant->max_state = 175;
        plant->kappa = 0.30;
        plant->unit_consume[electricity] = 3;
        plant->zeta_f = zeta_water;
        plant->sigma = 0.001;
        break;

        case water_tank:
        plant->output_type = water;
        plant->eta = 0.98;
        plant->max_state = 250;
        plant->kappa = 0.50;
        plant->max_capacity = 9000;
        plant->unit_consume[water] = 1;
        plant->sigma = 0.0002;
        break;

        case air_processor:
        plant->output_type = air;
        plant->eta = 0.95;
        plant->max_state = 90;
        plant->kappa = 0.40;
        plant->unit_consume[electricity] = 4;
        plant->zeta_f = zeta_air;
        plant->sigma = 0.001;
        break;

        case air_tank:
        plant->output_type = air;
        plant->eta = 0.98;
        plant->max_state = 150;
        plant->kappa = 0.60;
        plant->max_capacity = 7500;
        plant->unit_consume[air] = 1;
        plant->sigma = 0.0002;
        break;

        case greenhouse:
        plant->output_type = food;
        plant->eta = 0.95;
        plant->max_state = 75;
        plant->kappa = 0.10;
        plant->unit_consume[electricity] = 0.8;
        plant->unit_consume[heat] = 1.2;
        plant->unit_consume[water] = 1.3;
        plant->unit_consume[air] = 0.3;
        plant->sigma = 0.001;
        break;

        case food_storage:
        plant->output_type = food;
        plant->eta = 0.98;
        plant->max_state = 80;
        plant->kappa = 0.35;
        plant->max_capacity = 2500;
        plant->unit_consume[food] = 1;
        plant->sigma = 0.0002;
        break;

        case plantation:
        plant->output_type = biomass;
        plant->eta = 0.95;
        plant->max_state = 11;
        plant->kappa = 0.15;
        plant->zeta_f = zeta_growth;
        plant->sigma = 0.001;
        break;

        case biomass_silo:
        plant->output_type = biomass;
        plant->eta = 0.98;
        plant->max_state = 50;
        plant->kappa = 0.30;
        plant->max_capacity = 6000;
        plant->unit_consume[biomass] = 1;
        plant->sigma = 0.0002;
        break;

        case yeaster:
        plant->output_type = elan;
        plant->eta = 0.95;
        plant->max_state = 9;
        plant->kappa = 0.10;
        plant->zeta_f = zeta_growth;
        plant->sigma = 0;
        break;

        case distillery:
        plant->output_type = elan;
        plant->eta = 0.95;
        plant->max_state = 33;
        plant->kappa = 0.20;
        plant->unit_consume[electricity] = 5;
        plant->unit_consume[heat] = 3;
        plant->unit_consume[biomass] = 1;
        plant->sigma = 0.001;
        break;

        case elan_tank:
        plant->output_type = elan;
        plant->eta = 0.98;
        plant->max_state = 100;
        plant->kappa = 0.50;
        plant->max_capacity = 5000;
        plant->unit_consume[elan] = 1;
        plant->sigma = 0.0002;
        break;

        case computer:
        plant->output_type = cpu_power;
        plant->eta = 0.95;
        plant->max_state = 4;
        plant->kappa = 0.80;
        plant->unit_consume[electricity] = 1.5;
        plant->sigma = 0.0003;
        break;

        case habitat:
        plant->output_type = human_life_support;
        plant->eta = 0.95;
        plant->current_output = HABITAT_DEFAULT_PEOPLE;
        plant->max_state = HABITAT_MAX_PEOPLE;
        plant->kappa = 0.10;
        plant->max_capacity = plant->current_capacity = HABITAT_MAX_CAPACITY;
        plant->unit_consume[electricity] = 1;
        plant->unit_consume[heat] = 1;
        plant->unit_consume[water] = 1;
        plant->unit_consume[air] = 1;
        plant->unit_consume[food] = 1;
        plant->sigma = 0.0001;
        break;

        case hybridome:
        plant->output_type = hybrid_life_support;
        plant->eta = 0.95;
        plant->max_state = HYBRIDOME_MAX_PEOPLE;
        plant->kappa = 0.10;
        plant->max_capacity = HYBRIDOME_MAX_CAPACITY;
        plant->unit_consume[electricity] = 1;           /* Unica limitazione di durata */
        plant->unit_consume[elan] = 1;
        plant->sigma = 0;
        break;

        case metamorpher:
        plant->output_type = morphing_power;
        plant->eta = 0.95;
        plant->max_state = 1;
        plant->kappa = 0.10;
        plant->unit_consume[electricity] = 1;
        plant->unit_consume[heat] = 1;
        plant->unit_consume[air] = 1;
        plant->unit_consume[elan] = 3;
        plant->sigma = 0.001;
        break;

        case radiator:
        plant->output_type = BARRIER_RESOURCE;
        plant->eta = 0.95;
        plant->max_state = 3 / BARRIER_FACTOR;
        plant->kappa = 1;
        plant->unit_consume[BARRIER_RESOURCE] = 1;
        plant->sigma = 0.0003;
        break;

        default:
        assert(0);
        break;
        }

    plant->available_output = plant->surplus_output = plant->current_output;
    plant->current_state = plant->current_output / plant->eta;

    return plant;
    }

device *install_device(device_class class, cell *loc)
    {
    device *plant;
    device_id plant_id;

    if(sys.tot_plants == MAX_DEVICE_NUMBER)
        {
        display_message(too_many_devices);
        return NULL;
        }

    if(loc->plant || loc->alfa_duct || loc->turtle)
        {
        display_message(square_not_empty);
        return NULL;
        }

    plant_id.class = class;
    plant_id.num = sys.num_plants[class];

    plant = init_device(plant_id, loc);
    if(plant)
        {
        loc->plant = plant;
        loc->explored = TRUE;

        sys.plant[sys.tot_plants] = plant;
        sys.tot_plants++;
        sys.num_plants[class]++;
        qsort(sys.plant, sys.tot_plants, sizeof(*sys.plant),
                (int (*)(const void *, const void *))device_priority);

        set_device_icon(plant);

        explore_neighbors(loc);
        link_neighbors(loc);
        }

    return plant;
    }

void remove_device(device *plant)
    {
    channel *duct;
    boolean linked_duct;
    int p, d;
    direction dir;
    cell *near_loc;

    plant->loc->plant = NULL;
    clear_cell(plant->loc);

    for(p = 0; p < sys.tot_plants; p++)
        {
        if(sys.plant[p] == plant)
            {
            sys.plant[p] = NULL;
            qsort(sys.plant, sys.tot_plants, sizeof(*sys.plant),
                    (int (*)(const void *, const void *))device_priority);
            sys.tot_plants--;
            break;
            }
        }

    for(d = 0; d < sys.tot_ducts; d++)
        {
        duct = sys.duct[d];
        linked_duct = FALSE;

        for(p = 0; p < MAX_PROVIDERS_NUMBER; p++)
            {
            if(!duct->provider[p])
                break;

            if(duct->provider[p] == plant)
                {
                duct->provider[p] = NULL;   /* Niente break! */
                linked_duct = TRUE;
                }
            }
        if(linked_duct)
            qsort(duct->provider, p, sizeof(*duct->provider),
                    (int (*)(const void *, const void *))device_priority);

        if(linked_duct || duct == plant->input[duct->throughput_type])
            {
            for(dir = 0; dir < DIRECTION_NUMBER; dir++)
                {
                near_loc = neighbor(plant->loc, dir);
                if(near_loc->alfa_duct == duct && near_loc->num_links)
                    {
                    near_loc->num_links--;
                    /*** Bug di difficile soluzione: la locazione aggiornata
                    potrebbe non essere quella giusta, in caso di impianti
                    vicini; il caso non si presenta di frequente ***/

                    if(near_loc->num_links)
                        set_icon(&near_loc->sym, LINK_ICON(near_loc->num_links));
                    else
                        set_icon(&near_loc->sym, CORNER_ICON);

                    break;
                    }
                }
            }
        }

    free(plant);
    }

real eval_cell(program *mcp, cell *loc)
    {
    real hf;

    hf = mcp->ground_tropism * loc->ground_factor;

    if(!loc->explored)
        hf += mcp->unexplored_tropism;

    if(loc->plant)
        hf += mcp->device_tropism;

    if(loc->alfa_duct)
        hf += mcp->channel_tropism;

    if(loc->turtle)
        hf += mcp->mobile_tropism;

    return hf;
    }

cell *select_cell(program *mcp, cell *loc_1, cell *loc_2)
    {
    real hf_1, hf_2;

    hf_1 = eval_cell(mcp, loc_1);
    hf_2 = eval_cell(mcp, loc_2);

    if(hf_1 > hf_2)
        {
        if(mcp->selection_wits * exp(hf_2 - hf_1) < random(1))
            return loc_1;
        }
    else
        if(hf_1 < hf_2)
            {
            if(mcp->selection_wits * exp(hf_1 - hf_2) < random(1))
                return loc_2;
            }
        else
            if(mcp->selection_wits < random(1))
                return NULL;

    return (mcp->selection_bias < random(1))? loc_1 : loc_2;
    }

void upload_program(program_tag tag, mobile *turtle)
    {
    turtle->mcp.tag = tag;

    switch(tag)
        {
        case survey:
        turtle->mcp.selection_wits = 0;
        turtle->mcp.selection_bias = 0;
        turtle->mcp.ground_tropism = 0;
        turtle->mcp.unexplored_tropism = 0;
        turtle->mcp.device_tropism = 0;
        turtle->mcp.channel_tropism = 0;
        turtle->mcp.mobile_tropism = 1;
        break;

        case explore:
        turtle->mcp.selection_wits = 0.50;
        turtle->mcp.selection_bias = 0.50;
        turtle->mcp.ground_tropism = 0.1;
        turtle->mcp.unexplored_tropism = 1;
        turtle->mcp.device_tropism = -1;
        turtle->mcp.channel_tropism = -1;
        turtle->mcp.mobile_tropism = 0;
        break;

        case hunt:
        turtle->mcp.selection_wits = 0.25;
        turtle->mcp.selection_bias = 0.25;
        turtle->mcp.ground_tropism = 1;
        turtle->mcp.unexplored_tropism = 0;
        turtle->mcp.device_tropism = -1;
        turtle->mcp.channel_tropism = -1;
        turtle->mcp.mobile_tropism = 1;
        break;

        case patrol:
        turtle->mcp.selection_wits = 0.25;
        turtle->mcp.selection_bias = 0.25;
        turtle->mcp.ground_tropism = -1;
        turtle->mcp.unexplored_tropism = -1;
        turtle->mcp.device_tropism = -1;
        turtle->mcp.channel_tropism = 0;
        turtle->mcp.mobile_tropism = 1;
        break;

        case wander:
        turtle->mcp.selection_wits = 1;
        turtle->mcp.selection_bias = 1;
        turtle->mcp.ground_tropism = 0;
        turtle->mcp.unexplored_tropism = 0;
        turtle->mcp.device_tropism = 0;
        turtle->mcp.channel_tropism = 0;
        turtle->mcp.mobile_tropism = 0;
        break;

        case pantropic_cp:
        turtle->mcp.selection_wits = 0.75;
        turtle->mcp.selection_bias = 0.75;
        turtle->mcp.ground_tropism = 3;
        turtle->mcp.unexplored_tropism = 0;
        turtle->mcp.device_tropism = 0;
        turtle->mcp.channel_tropism = 0;
        turtle->mcp.mobile_tropism = 0;
        break;

        case mutant_cp:
        turtle->mcp.selection_wits = 1;
        turtle->mcp.selection_bias = 1;
        turtle->mcp.ground_tropism = 0;
        turtle->mcp.unexplored_tropism = 0;
        turtle->mcp.device_tropism = 0;
        turtle->mcp.channel_tropism = 0;
        turtle->mcp.mobile_tropism = 0;
        break;

        case parasite_cp:
        turtle->mcp.selection_wits = 0.75;
        turtle->mcp.selection_bias = 0.75;
        turtle->mcp.ground_tropism = 3;
        turtle->mcp.unexplored_tropism = 0;
        turtle->mcp.device_tropism = 0;
        turtle->mcp.channel_tropism = 0;
        turtle->mcp.mobile_tropism = 0;
        break;

        case breeder_cp:
        turtle->mcp.selection_wits = 0.75;
        turtle->mcp.selection_bias = 0.75;
        turtle->mcp.ground_tropism = 3;
        turtle->mcp.unexplored_tropism = 1;
        turtle->mcp.device_tropism = 0;
        turtle->mcp.channel_tropism = 0;
        turtle->mcp.mobile_tropism = 0;
        break;

        case gizmo_cp:
        turtle->mcp.selection_wits = 1;
        turtle->mcp.selection_bias = 0.33;
        turtle->mcp.ground_tropism = 0;
        turtle->mcp.unexplored_tropism = 0;
        turtle->mcp.device_tropism = 0;
        turtle->mcp.channel_tropism = 0;
        turtle->mcp.mobile_tropism = 0;
        break;

        case mole_cp:
        turtle->mcp.selection_wits = 0.75;
        turtle->mcp.selection_bias = 0.75;
        turtle->mcp.ground_tropism = -3;
        turtle->mcp.unexplored_tropism = 0;
        turtle->mcp.device_tropism = 0;
        turtle->mcp.channel_tropism = 0;
        turtle->mcp.mobile_tropism = -1;
        break;

        default:
        assert(0);
        break;
        }
    }

cell *update_mobile(mobile *turtle)
    {
    cell *loc, *near_loc, *cmp_loc;
    direction dir;
    channel *duct;

    loc = turtle->loc;

    if(!turtle->active)
        near_loc = NULL;
    else
        {
        dir = floor(random(DIRECTION_NUMBER));
        near_loc = neighbor(loc, turtle->heading);
        cmp_loc = neighbor(loc, dir);

        near_loc = select_cell(&turtle->mcp, near_loc, cmp_loc);
        if(!near_loc)
            turtle->heading = floor(random(DIRECTION_NUMBER));
        else
            {
            if(near_loc == cmp_loc)
                turtle->heading = dir;

            if(near_loc->plant)
                {
                switch(turtle->device_reaction)
                    {
                    case bump:
                    turtle->heading = floor(random(DIRECTION_NUMBER));
                    near_loc = NULL;
                    break;

                    case attack:
                    attack_device(near_loc->plant);
                    break;

                    case jump:
                    near_loc = jump_obstacle(turtle);
                    break;

                    default:
                    near_loc = NULL;
                    break;
                    }
                }
            else
                if(near_loc->alfa_duct)
                    {
                    if(near_loc->break_flow)
                        near_loc = jump_obstacle(turtle);
                    else
                        {
                        duct = near_loc->alfa_duct;
                        switch(turtle->channel_reaction)
                            {
                            case bump:
                            turtle->heading = floor(random(DIRECTION_NUMBER));
                            near_loc = NULL;
                            break;

                            case attack:
                            if(duct->throughput_type != BARRIER_RESOURCE ||
                                random(1) < exp(-BARRIER_FACTOR * duct->current_throughput))
                                attack_channel(duct, near_loc);
                            else
                                {
                                turtle->heading = floor(random(DIRECTION_NUMBER));
                                near_loc = NULL;
                                }
                            break;

                            case jump:
                            near_loc = jump_obstacle(turtle);
                            break;

                            default:
                            near_loc = NULL;
                            break;
                            }
                        }
                    }
                else
                    if(near_loc->turtle)
                        {
                        switch(turtle->mobile_reaction[near_loc->turtle->class])
                            {
                            case bump:
                            turtle->heading = floor(random(DIRECTION_NUMBER));
                            near_loc = NULL;
                            break;

                            case attack:
                            attack_mobile(turtle, near_loc->turtle);
                            break;

                            case mate:
                            mate_mobile(turtle, near_loc->turtle);
                            break;

                            case jump:
                            near_loc = jump_obstacle(turtle);
                            break;

                            default:
                            near_loc = NULL;
                            break;
                            }
                        }
                    else
                        move_mobile(turtle, near_loc);
            }
        }

    if(loc == turtle->loc)              /* Non ci sono stati spostamenti */
        update_mobile_icon(turtle);

    return near_loc;
    }

void attack_device(device *plant)
    {
    damage_device(plant);
    update_device_icon(plant);
    display_message(system_attacked);
    }

void attack_channel(channel *duct, cell *loc)
    {
    if(!loc->beta_duct && !loc->break_flow && !loc->num_links)
        {
        if(!duct->break_loc)
            loc->break_flow = loc;
        else
            {
            loc->break_flow = duct->break_loc->break_flow;
            duct->break_loc->break_flow = loc;
            }
        duct->break_loc = loc;
        duct->current_throughput = 0;

        set_attribute(&loc->sym, broken_channel);
        display_message(channel_attacked);
        }
    }

mobile *init_mobile(mobile_class class, cell *loc)
    {
    mobile *turtle;
    mobile_class def_class;

    turtle = calloc(1, sizeof(*turtle));
    assert(turtle != NULL);

    turtle->class = class;
    turtle->loc = loc;
    turtle->heading = floor(random(DIRECTION_NUMBER));
    turtle->eta = 1;

    if(mobile_max_age[class] <= MIN_TIME_GRANTED)
        turtle->time_to_remove = mobile_max_age[class];
    else
        turtle->time_to_remove = MIN_TIME_GRANTED +
                                    floor(random(mobile_max_age[class] - MIN_TIME_GRANTED));

    for(def_class = 0; def_class < MOBILE_CLASS_NUMBER; def_class++)
        if(csi[class][def_class])
            turtle->mobile_reaction[def_class] = attack;
        else
            turtle->mobile_reaction[def_class] = bump;

    switch(class)
        {
        case roboprobe:
        turtle->active = TRUE;
        upload_program(explore, turtle);
        turtle->device_reaction = bump;
        turtle->channel_reaction = bump;
        turtle->explore_cell = TRUE;
        break;

        case raider:
        turtle->remote_control = TRUE;
        upload_program(survey, turtle);
        turtle->device_reaction = bump;
        turtle->channel_reaction = bump;
        turtle->explore_cell = TRUE;
        break;

        case exterminator:
        turtle->remote_control = TRUE;
        upload_program(survey, turtle);
        turtle->device_reaction = bump;
        turtle->channel_reaction = bump;
        turtle->explore_cell = TRUE;
        break;

        case stalker:
        turtle->remote_control = TRUE;
        upload_program(survey, turtle);
        turtle->device_reaction = bump;
        turtle->channel_reaction = jump;
        turtle->explore_cell = TRUE;
        break;

        case guardian:
        turtle->active = TRUE;
        upload_program(patrol, turtle);
        turtle->device_reaction = bump;
        turtle->channel_reaction = jump;
        turtle->explore_cell = TRUE;
        break;

        case pantropic_form:
        turtle->active = TRUE;
        upload_program(pantropic_cp, turtle);
        turtle->device_reaction = bump;
        turtle->channel_reaction = bump;
        turtle->can_breed = (random(1) < 0.5);
        if(!turtle->can_breed)
            turtle->mobile_reaction[pantropic_form] = mate;
        break;

        case mutant_form:
        turtle->active = TRUE;
        upload_program(mutant_cp, turtle);
        turtle->device_reaction = attack;
        turtle->channel_reaction = attack;
        break;

        case alien_parasite:
        turtle->active = TRUE;
        upload_program(parasite_cp, turtle);
        turtle->device_reaction = attack;
        turtle->channel_reaction = bump;
        turtle->mobile_reaction[alien_breeder] = mate;
        break;

        case alien_breeder:
        turtle->active = TRUE;
        upload_program(breeder_cp, turtle);
        turtle->device_reaction = attack;
        turtle->channel_reaction = attack;
        turtle->can_breed = TRUE;
        break;

        case plasma_gizmo:
        turtle->active = TRUE;
        upload_program(gizmo_cp, turtle);
        turtle->device_reaction = bump;
        turtle->channel_reaction = attack;
        turtle->can_breed = TRUE;
        turtle->steps_to_breed = turtle->time_to_remove / 3;
        break;

        case alien_mole:
        turtle->active = TRUE;
        upload_program(mole_cp, turtle);
        turtle->device_reaction = attack;
        turtle->channel_reaction = jump;
        break;

        case space_shuttle:
        turtle->active = TRUE;
        upload_program(survey, turtle);
        break;

        default:
        assert(0);
        break;
        }

    return turtle;
    }

mobile *install_mobile(mobile_class class, cell *loc)
    {
    mobile *turtle;

    if(sys.tot_turtles == MAX_MOBILE_NUMBER)
        {
        display_message(too_many_mobiles);
        return NULL;
        }

    if(loc->plant || loc->alfa_duct || loc->turtle)
        {
        display_message(square_not_empty);
        return NULL;
        }

    turtle = init_mobile(class, loc);
    if(turtle)
        {
        loc->turtle = turtle;
        if(turtle->explore_cell)
            loc->explored = TRUE;

        set_mobile_icon(turtle);

        sys.turtle[sys.tot_turtles] = turtle;
        sys.tot_turtles++;
        sys.sort_turtles = TRUE;

        if(turtle->class < PRIMARY_MC_NUMBER)
            usr.robots++;
        else
            switch(turtle->class)
                {
                case pantropic_form:
                usr.pantropics++;
                break;

                case mutant_form:
                case alien_parasite:
                case alien_breeder:
                case plasma_gizmo:
                case alien_mole:
                usr.hostiles++;
                break;

                case space_shuttle:
                break;

                default:
                assert(0);
                break;
                }
        }
    return turtle;
    }

void remove_mobile(mobile *turtle)
    {
    int t;

    turtle->loc->turtle = NULL;
    clear_cell(turtle->loc);

    for(t = 0; t < sys.tot_turtles; t++)
        {
        if(sys.turtle[t] == turtle)
            {
            sys.turtle[t] = NULL;
            sys.sort_turtles = TRUE;    /* tot_turtles viene decrementato dopo il sort */
            break;
            }
        }

    if(turtle->class < PRIMARY_MC_NUMBER)
        usr.robots--;
    else
        switch(turtle->class)
            {
            case pantropic_form:
            usr.pantropics--;
            break;

            case mutant_form:
            case alien_parasite:
            case alien_breeder:
            case plasma_gizmo:
            case alien_mole:
            usr.hostiles--;
            break;

            case space_shuttle:
            break;

            default:
            assert(0);
            break;
            }

    free(turtle);
    }

real update_device(device *plant)
    {
    real actual_input, available_input, requested_input;
    real storage_output;
    real target_state, delta_state, next_state;
    real zeta;

    if(!plant->broken)
        {
        assert(plant->surplus_output <= plant->available_output ||
                plant->output_type > PRIMARY_RT_NUMBER);

        /* Si recupera il recuperabile */

        plant->current_capacity = min(plant->max_capacity,
                                plant->current_capacity + plant->available_output / plant->eta);

        /* Si decide un nuovo obiettivo */

        if(plant->demand_driven)
            plant->target_output = max(0, min(plant->max_state,
                                                plant->current_output - plant->surplus_output));
        /* Si calcola il nuovo stato */

        target_state = min(plant->max_state, plant->target_output / plant->eta);
        delta_state = plant->kappa * (target_state - plant->current_state);
        next_state = max(0, plant->current_state + delta_state);

        requested_input = next_state + plant->max_capacity - plant->current_capacity;
        zeta = calc_zeta(plant->zeta_f, plant->loc);
        if(zeta)
            requested_input /= zeta;
        else
            requested_input = 0;    /* Si rinuncia */

        requested_input = min(plant->max_state, requested_input);
        available_input = poll_providers(plant, requested_input, demand);
        assert(available_input <= requested_input);

        /* E` sempre garantito che actual_input <= available_input <= requested_input */
        actual_input = zeta * available_input;
        storage_output = next_state - actual_input;

        if(storage_output <= plant->current_capacity)   /* Sistema libero */
            {
            plant->current_state = next_state;
            plant->current_capacity =
                min(plant->max_capacity, plant->current_capacity - storage_output);
            /* Per ragioni numeriche, quando actual_input = requested_input e zeta < 1,
                current_capacity poteva essere un eps > 0 anche se max_capacity = 0; fixed */
            }
        else
            {                                           /* Sistema vincolato */
            target_state = actual_input + plant->current_capacity;
            delta_state = plant->kappa * (target_state - plant->current_state);
            next_state = max(0, plant->current_state + delta_state);

            storage_output = next_state - actual_input;

            plant->current_state = next_state;
            plant->current_capacity = min(plant->max_capacity,
                max(0, plant->current_capacity - storage_output));
            }

        poll_providers(plant, available_input, commit);

        plant->tau = min(1, plant->tau +
        (plant->sigma * (abs(delta_state) + RHO * plant->current_state) / plant->max_state));

        if(random(1) < plant->tau)
            {
            damage_device(plant);
            display_message(system_failure);
            }
        else
            {
            plant->current_output = plant->eta * plant->current_state;
            plant->available_output = plant->surplus_output = plant->current_output;
            }

        update_device_icon(plant);
        }

    return plant->current_output;
    }

void damage_device(device *plant)
    {
    plant->broken = TRUE;
    plant->current_state = 0;
    plant->current_capacity -= random(random(plant->current_capacity));
    plant->eta *= 1 - random(plant->tau);
    plant->tau = random(plant->tau);

    plant->available_output = plant->surplus_output = plant->current_output = 0;
    }

real poll_providers(device *plant, real request, request_mode mode)
    {
    resource_type g, c, num_constraints;
    channel *duct, *constraint[PRIMARY_RT_NUMBER];
    real consume, consume_to_go, deficit[PRIMARY_RT_NUMBER], ratio, new_ratio;
    device *provider;
    int p;

    ratio = 1;
    num_constraints = 0;
    if(request)
        {
        for(g = 0; g < PRIMARY_RT_NUMBER; g++)
            {
            consume = plant->unit_consume[g] * request;
            if(consume)
                {
                duct = plant->input[g];
                if(!duct || duct->break_loc)
                    {
                    ratio = 0;
                    num_constraints = 0;
                    break;
                    }

                consume_to_go = consume;
                for(p = 0; p < MAX_PROVIDERS_NUMBER && consume_to_go > 0; p++)
                    {
                    provider = duct->provider[p];
                    if(!provider)
                        break;

                    if(plant->output_type != g || duct != provider->input[g])
                        {
                        switch(mode)
                            {
                            case demand:
                            if(consume_to_go <= provider->surplus_output)
                                {
                                provider->surplus_output -= consume_to_go;
                                consume_to_go = 0;
                                }
                            else
                                {
                                if(consume_to_go <= provider->available_output)
                                    consume_to_go = 0;
                                else
                                    consume_to_go -= provider->available_output;

                                if(provider->surplus_output > 0)
                                    provider->surplus_output = 0;
                                }
                            break;

                            case commit:
                            if(consume_to_go <= provider->available_output)
                                {
                                provider->available_output -= consume_to_go;
                                consume_to_go = 0;
                                }
                            else
                                {
                                consume_to_go -= provider->available_output;
                                provider->available_output = 0;
                                }
                            break;

                            default:
                            assert(0);
                            break;
                            }
                        }
                    }

                switch(mode)
                    {
                    case demand:
                    duct->target_throughput += consume;
                    if(consume_to_go)
                        {
                        new_ratio = 1 - consume_to_go / consume;
                        if(new_ratio < ratio + CONTROL_TOLL)
                            {
                            if(new_ratio < ratio)
                                {
                                if(new_ratio < ratio - CONTROL_TOLL)
                                    num_constraints = 0;
                                ratio = new_ratio;
                                }
                            constraint[num_constraints] = duct;
                            deficit[num_constraints] = consume_to_go;
                            num_constraints++;
                            }
                        }
                    break;

                    case commit:
                    duct->current_throughput += consume - consume_to_go;
                    if(consume_to_go)
                        {
                        new_ratio = 1 - consume_to_go / consume;
                        if(new_ratio < ratio)
                            ratio = new_ratio;
                        }
                    break;

                    default:
                    assert(0);
                    break;
                    }
                }
            }

        if(mode == demand)
            {
            for(c = 0; c < num_constraints; c++)
                {
                g = constraint[c]->throughput_type;
                constraint[c]->demanded_throughput += deficit[c];

                for(p = 0; p < MAX_PROVIDERS_NUMBER; p++)
                    {
                    provider = constraint[c]->provider[p];
                    if(!provider)
                        break;

                    if(plant->output_type != g || constraint[c] != provider->input[g])
                        {
                        assert(provider->surplus_output <= 0);
                        provider->surplus_output -= deficit[c];
                        }
                    }
                }
            }
        }

    return (real)(ratio * request);
    }

void set_device_icon(device *plant)
    {
    set_icon(&plant->loc->sym, device_icon[plant->id.class]);
    set_color(&plant->loc->sym, resource_color[plant->output_type]);
    update_device_icon(plant);
    }

void update_device_icon(device *plant)
    {
    if(plant->broken)
        set_attribute(&plant->loc->sym, broken_device);
    else
        if(plant->max_capacity)
            {
            if(plant->current_capacity > USER_EPS * plant->max_capacity)
                set_attribute(&plant->loc->sym, full_device);
            else
                set_attribute(&plant->loc->sym, empty_device);
            }
        else
            {
            if(plant->current_output > USER_EPS * plant->max_state)
                set_attribute(&plant->loc->sym, active_device);
            else
                set_attribute(&plant->loc->sym, idle_device);
            }
    }

void set_mobile_icon(mobile *turtle)
    {
    if(turtle->loc->explored)
        {
        set_icon(&turtle->loc->sym, mobile_icon[turtle->class]);
        set_color(&turtle->loc->sym, mobile_color[turtle->class]);
        update_mobile_icon(turtle);
        }
    }

void update_mobile_icon(mobile *turtle)
    {
    if(turtle->loc->explored)
        {
        if(turtle->steps_to_breed)
            set_attribute(&turtle->loc->sym, gravid_mobile);
        else
            if(turtle->active)
                set_attribute(&turtle->loc->sym, active_mobile);
            else
                set_attribute(&turtle->loc->sym, idle_mobile);
        }
    }

void update_environment()
    {
    real base, solar_factor, temperature_factor, air_factor, water_factor, growth_factor;

    env.time++;
    base = 0.5 + BASE_RANGE * sin(OMEGA * env.time);

    solar_factor = base + random(2 * SOLAR_RANGE) - SOLAR_RANGE;
    temperature_factor = solar_factor + random(2 * TEMPERATURE_RANGE) - TEMPERATURE_RANGE;

    air_factor = temperature_factor + random(2 * AIR_RANGE) - AIR_RANGE;
    water_factor = temperature_factor + random(2 * WATER_RANGE) - WATER_RANGE;
    growth_factor = 1 - (temperature_factor + random(2 * GROWTH_RANGE) - GROWTH_RANGE);

    env.solar_factor = max(0, min(1, (env.solar_factor + solar_factor) / 2));
    env.temperature_factor = max(0, min(1, (env.temperature_factor + temperature_factor) / 2));
    env.air_factor = max(0, min(1, (env.air_factor + air_factor) / 2));
    env.water_factor = max(0, min(1, (env.water_factor + water_factor) / 2));
    env.growth_factor = max(0, min(1, (env.growth_factor + growth_factor) / 2));
    }

real calc_zeta(zeta_factor zeta_f, cell *loc)
    {
    switch(zeta_f)
        {
        case zeta_1:
        return 1;
        break;

        case zeta_solar:
        return env.solar_factor;
        break;

        case zeta_temperature:
        return env.temperature_factor;
        break;

        case zeta_water:
        return pow(env.water_factor * (1 - loc->ground_factor), 0.5);
        break;

        case zeta_air:
        return pow(env.air_factor * (1 - loc->ground_factor), 0.5);
        break;

        case zeta_growth:
        return pow(env.growth_factor * loc->ground_factor, 0.5);
        break;

        default:
        assert(0);
        break;
        }
    return 0;
    }

void start_game(cell *loc)
    {
    int k;

    install_device(habitat, loc);
    display_cell(loc);
    display_neighbors(loc);

    place_near(space_shuttle, loc);
    for(k = 0; k < INITIAL_PARASITES; k++)
        place_mobile(alien_parasite);
    for(k = 0; k < INITIAL_BREEDERS; k++)
        place_mobile(alien_breeder);
    /* Almeno una breeder ha vita uguale alla sua vita media */
    sys.turtle[sys.tot_turtles - 1]->time_to_remove = mobile_max_age[alien_breeder] / 2;
    for(k = 0; k < INITIAL_GIZMOS; k++)
        place_mobile(plasma_gizmo);
    for(k = 0; k < INITIAL_MOLES; k++)
        place_mobile(alien_mole);

    display_frame();
    display_message(title_message);
    }

void end_game()
    {
    int k;

    lock_interface();
    for(k = 0; k < MANY_YEARS && !receive_interrupt(); k++)
        {
        do
            next_day(FALSE);
        while(env.time % BASE_PERIOD);

        display_map();
        display_frame();
        }
    unlock_interface();

    display_message(end_of_game);
    }

cell *place_mobile(mobile_class class)
    {
    cell *loc;

    do
        loc = &env.map[(int)floor(random(MAP_ROWS))][(int)floor(random(MAP_COLS))];
    while(loc->plant || loc->alfa_duct || loc->turtle);

    install_mobile(class, loc);
    display_cell(loc);

    return loc;
    }

cell *place_near(mobile_class class, cell *loc)
    {
    direction dir;
    cell *near_loc;

    for(dir = 0; dir < DIRECTION_NUMBER; dir++)
        {
        near_loc = neighbor(loc, dir);
        if(!near_loc->plant && !near_loc->alfa_duct && !near_loc->turtle)
            {
            install_mobile(class, near_loc);
            display_cell(near_loc);
            return near_loc;
            }
        }
    return NULL;
    }

int place_around_providers(int num, mobile_class class, resource_type output_type)
    {
    device *plant;
    int to_place, p;

    p = 0;
    plant = NULL;
    to_place = num;
    while(to_place)
        {
        while(!plant && p < sys.tot_plants)
            {
            if(sys.plant[p]->output_type != output_type || sys.plant[p]->broken)
                p++;
            else
                plant = sys.plant[p];
            }

        if(!plant)
            break;

        if(place_near(class, plant->loc))
            to_place--;
        else
            {
            plant = NULL;
            p++;
            }
        }

    return (num - to_place);
    }

boolean has_around_plants(cell *loc)
    {
    direction dir;
    cell *near_loc;

    for(dir = 0; dir < DIRECTION_NUMBER; dir++)
        {
        near_loc = neighbor(loc, dir);
        if(near_loc->plant)
            return TRUE;
        }
    return FALSE;
    }

boolean has_around_providers(cell *loc, resource_type output_type)
    {
    direction dir;
    cell *near_loc;

    for(dir = 0; dir < DIRECTION_NUMBER; dir++)
        {
        near_loc = neighbor(loc, dir);
        if(near_loc->plant &&
            near_loc->plant->output_type == output_type && !near_loc->plant->broken)
            return TRUE;
        }
    return FALSE;
    }

void attack_mobile(mobile *attacker, mobile *defender)
    {
    if(csi[attacker->class][defender->class])
        {
        defender->eta = max(0, defender->eta - 1 / csi[attacker->class][defender->class]);
        if(!defender->eta)
            {
            defender->time_to_remove = 1;
            if(defender->class < PRIMARY_MC_NUMBER)
                display_message(robot_terminated);
            else
                {
                switch(defender->class)
                    {
                    case pantropic_form:
                    display_message(pantropic_terminated);
                    break;

                    case mutant_form:
                    display_message(mutant_terminated);
                    break;

                    case alien_parasite:
                    case alien_breeder:
                    case plasma_gizmo:
                    case alien_mole:
                    display_message(alien_terminated);
                    break;

                    case space_shuttle:
                    break;

                    default:
                    assert(0);
                    break;
                    }
                }
            }
        defender->steps_to_breed = 0;
        if(defender->loc->explored)
            set_attribute(&defender->loc->sym, hit_mobile);
        }
    }

void move_mobile(mobile *turtle, cell *loc)
    {
    cell *prev_loc;

    prev_loc = turtle->loc;
    turtle->loc = loc;

    prev_loc->turtle = NULL;
    clear_cell(prev_loc);

    loc->turtle = turtle;
    if(turtle->explore_cell)
        loc->explored = TRUE;

    set_mobile_icon(turtle);

    if(turtle->steps_to_breed)
        {
        turtle->steps_to_breed--;
        if(!turtle->steps_to_breed)
            breed_mobile(turtle, prev_loc);
        }
    }

cell *jump_obstacle(mobile *turtle)
    {
    cell *near_loc;

    near_loc = neighbor(neighbor(turtle->loc, turtle->heading), turtle->heading);
    if(!near_loc->plant && !near_loc->alfa_duct && !near_loc->turtle)
        {
        move_mobile(turtle, near_loc);
        return near_loc;
        }
    else
        return NULL;
    }

void mate_mobile(mobile *father, mobile *mother)
    {
    if(mother->can_breed && !mother->steps_to_breed && random(1) < env.growth_factor)
        {
        switch(mother->class)
            {
            case pantropic_form:
            mother->steps_to_breed = PANTROPIC_STEPS_TO_BREED;
            break;

            case alien_breeder:
            mother->steps_to_breed = ALIEN_STEPS_TO_BREED;
            break;

            default:
            assert(0);
            break;
            }
        }
    }

void breed_mobile(mobile *mother, cell *loc)
    {
    switch(mother->class)
        {
        case pantropic_form:
        install_mobile((random(1) < MUTATION_RATIO)?
                        mutant_form : pantropic_form, loc);
        break;

        case alien_breeder:
        install_mobile((random(1) < ALIEN_FORMS_RATIO)?
                        alien_breeder : alien_parasite, loc);
        break;

        case plasma_gizmo:
        install_mobile(plasma_gizmo, loc);
        break;

        default:
        assert(0);
        break;
        }
    }

