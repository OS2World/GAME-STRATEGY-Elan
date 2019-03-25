
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef enum zeta_factor
    {
    zeta_1,
    zeta_solar,
    zeta_temperature,
    zeta_air,
    zeta_water,
    zeta_growth,
    ZETA_FACTOR_NUMBER
    } zeta_factor;

typedef enum request_mode
    {
    demand,
    commit,
    REQUEST_MODE_NUMBER
    } request_mode;

typedef enum behaviour
    {
    ignore,
    bump,
    attack,
    mate,
    jump,
    BEHAVIOURS_NUMBER
    } behaviour;

typedef enum event
    {
    no_news,
    extension_failure,
    PRIMARY_EVENT_NUMBER,   /* Da qui in poi implicano il trascorrere del tempo */
    time_passed,
    shuttle_arrival,
    shuttle_departure,
    colony_end,
    EVENT_NUMBER
    } event;

typedef struct channel
    {
    /* I primi due fungono da id */
    resource_type throughput_type;
    int num;
    struct cell *start_loc;
    struct cell *end_loc;
    struct cell *break_loc;
    int length;
    real current_throughput;
    real target_throughput;
    real demanded_throughput;
    struct device *provider[MAX_PROVIDERS_NUMBER];
    } channel;

typedef struct device_id
    {
    device_class class;
    int num;
    } device_id;

typedef struct device
    {
    device_id id;
    struct cell *loc;
    resource_type output_type;
    real current_output;
    real target_output;
    real available_output;  /* >= 0 */
    real surplus_output;    /* libero in segno (< 0 per output insufficiente) */
    boolean demand_driven;
    real eta;               /* 0 <= eta <= 1, eta = 1 per impianto ideale */
    real current_state;
    real max_state;
    real kappa;             /* 0 < kappa < 1 */
    real current_capacity;
    real max_capacity;
    real unit_consume[PRIMARY_RT_NUMBER];
    channel *input[PRIMARY_RT_NUMBER];
    zeta_factor zeta_f;     /* 0 <= calc_zeta(zeta_f, loc) <= 1, = 1 per ambiente ideale */
    real sigma;             /* Coefficiente di usura */
    real tau;               /* Break chance, 0 < tau < 1 */
    boolean broken;
    } device;

typedef struct program
    {
    program_tag tag;
    real selection_wits;
    real selection_bias;
    real ground_tropism;
    real unexplored_tropism;
    real device_tropism;
    real channel_tropism;
    real mobile_tropism;
    } program;

typedef struct mobile
    {
    mobile_class class;
    struct cell *loc;
    direction heading;
    boolean active;
    boolean remote_control;
    program mcp;
    behaviour device_reaction;
    behaviour channel_reaction;
    behaviour mobile_reaction[MOBILE_CLASS_NUMBER];
    boolean explore_cell;
    boolean can_breed;
    int steps_to_breed;             /* Numero di movimenti, non tempo assoluto */
    int time_to_remove;             /* Speranza di vita */
    real eta;
    } mobile;

typedef struct systems
    {
    device *plant[MAX_DEVICE_NUMBER];
    channel *duct[MAX_CHANNEL_NUMBER];
    mobile *turtle[MAX_MOBILE_NUMBER];
    int num_plants[DEVICE_CLASS_NUMBER];
    int num_ducts[PRIMARY_RT_NUMBER];
    real demand[SECONDARY_RT_NUMBER];
    real offer[SECONDARY_RT_NUMBER];
    int tot_plants;
    int tot_ducts;
    int tot_turtles;
    boolean sort_turtles;
    } systems;

typedef struct position
    {
    signed char i;
    signed char j;
    } position;

typedef struct cell
    {
    position pos;
    device *plant;
    channel *alfa_duct;
    channel *beta_duct;
    struct cell *alfa_flow;
    struct cell *beta_flow;
    struct cell *break_flow;
    int num_links;
    mobile *turtle;
    real ground_factor;
    boolean explored;
    symbol sym;
    } cell;

typedef struct environment
    {
    int time;
    cell map[MAP_ROWS][MAP_COLS];
    real solar_factor;
    real temperature_factor;
    real air_factor;
    real water_factor;
    real growth_factor;
    } environment;

typedef struct user
    {
    real money;
    int humans;
    int hybrids;
    int pantropics;
    int robots;
    int hostiles;
    real human_growing;
    real hybrid_growing;
    real pantropic_growing;
    int device_storage[DEVICE_CLASS_NUMBER];
    int mobile_storage[PRIMARY_MC_NUMBER];
    boolean shuttle_landed;
    boolean colony_dead;
    real score;
    } user;

extern const real device_price[DEVICE_CLASS_NUMBER];
extern const real resource_unit_price[PRIMARY_RT_NUMBER];
extern const real channel_unit_price[PRIMARY_RT_NUMBER];
extern const real mobile_price[PRIMARY_MC_NUMBER];
extern const int mobile_max_age[MOBILE_CLASS_NUMBER];
extern const real csi[MOBILE_CLASS_NUMBER][MOBILE_CLASS_NUMBER];

extern environment env;
extern systems sys;
extern user usr;

/* Protos */
device *init_device(device_id plant_id, cell *loc);
device *install_device(device_class class, cell *loc);
void remove_device(device *plant);
channel *init_channel(device *source_plant, int num, direction dir);
cell *extend_channel(channel *duct, cell *loc, direction dir);
channel *install_channel(device *source_plant, direction dir);
/* void remove_channel(channel *duct); */
void swap_channels(cell *loc);
mobile *init_mobile(mobile_class class, cell *loc);
mobile *install_mobile(mobile_class class, cell *loc);
void remove_mobile(mobile *turtle);
cell *place_mobile(mobile_class class);
cell *place_near(mobile_class class, cell *loc);
int place_around_providers(int num, mobile_class class, resource_type output_type);
boolean has_around_plants(cell *loc);
boolean has_around_providers(cell *loc, resource_type output_type);
boolean link_channel(channel *duct, device *plant);
boolean link_device(device *plant, channel *duct);
cell *neighbor(cell *loc, direction dir);
void link_neighbors(cell *loc);
void explore_neighbors(cell *loc);
void clear_cell(cell *loc);
void init_systems(void);
void init_user(void);
void init_environment(void);
real update_device(device *plant);
void damage_device(device *plant);
void attack_device(device *plant);
void attack_channel(channel *duct, cell *loc);
void attack_mobile(mobile *attacker, mobile *defender);
real poll_providers(device *plant, real request, request_mode mode);
cell *update_mobile(mobile *turtle);
cell *select_cell(program *mcp, cell *loc_1, cell *loc_2);
real eval_cell(program *mcp, cell *loc);
void upload_program(program_tag tag, mobile *turtle);
void update_environment(void);
void update_systems(boolean trace);
event next_day(boolean trace);
event next_year(boolean trace);
void set_device_icon(device *plant);
void update_device_icon(device *plant);
void set_mobile_icon(mobile *turtle);
void update_mobile_icon(mobile *turtle);
real trade_resource(device *plant, real requested_capacity);
void purchase_items(const char **name, const icon_type *icon, const real *price,
    int *storage, int class_number);
int device_priority(const device **plant_1, const device **plant_2);
int mobile_priority(const mobile **turtle_1, const mobile **turtle_2);
real calc_zeta(zeta_factor zeta_f, cell *loc);
real delta_people(real people, real life_support);
void move_mobile(mobile *turtle, cell *loc);
void mate_mobile(mobile *father, mobile *mother);
void breed_mobile(mobile *turtle, cell *loc);
cell *jump_obstacle(mobile *turtle);
void start_game(cell *loc);
void end_game(void);
void clean_exit(int rv);

