
#define MAP_ROWS    30
#define MAP_COLS    50

#define GROUND_AREA     (MAP_ROWS * MAP_COLS)
#define distance(p, q)  max(abs(p.i - q.i), abs(p.j - q.j))

/* Stime prudenti in base ai vincoli geometrici */
#define MAX_DEVICE_NUMBER       (GROUND_AREA / 4)
#define MAX_CHANNEL_NUMBER      (GROUND_AREA - MAX_DEVICE_NUMBER)
#define MAX_MOBILE_NUMBER       GROUND_AREA
#define MAX_PROVIDERS_NUMBER    MAX_DEVICE_NUMBER

typedef enum direction
    {
    N,
    S,
    W,
    E,
    NW,
    SW,
    NE,
    SE,
    DIRECTION_NUMBER
    } direction;

typedef enum resource_type
    {
    electricity,
    heat,
    water,
    air,
    food,
    biomass,
    elan,
    PRIMARY_RT_NUMBER,
    cpu_power,
    human_life_support,
    hybrid_life_support,
    morphing_power,
    RESOURCE_TYPE_NUMBER
    } resource_type;

#define SECONDARY_RT_BASE   (PRIMARY_RT_NUMBER + 1)
#define SECONDARY_RT_NUMBER (RESOURCE_TYPE_NUMBER - PRIMARY_RT_NUMBER - 1)

typedef enum device_class   /* L'ordinamento è rilevante ai fini delle priorità di produzione */
    {
    solar_cell,
    nuclear_reactor,
    battery,
    heating_system,
    water_processor,
    water_tank,
    air_processor,
    air_tank,
    greenhouse,
    food_storage,
    plantation,
    biomass_silo,
    yeaster,
    distillery,
    elan_tank,
    computer,
    habitat,
    hybridome,
    metamorpher,
    radiator,
    DEVICE_CLASS_NUMBER
    } device_class;

typedef enum mobile_class
    {
    roboprobe,
    raider,
    exterminator,
    stalker,
    guardian,
    PRIMARY_MC_NUMBER,
    pantropic_form,
    mutant_form,
    alien_parasite,
    alien_breeder,
    plasma_gizmo,
    alien_mole,
    space_shuttle,
    MOBILE_CLASS_NUMBER
    } mobile_class;

typedef enum program_tag
    {
    survey,
    explore,
    hunt,
    patrol,
    PRIMARY_PT_NUMBER,
    wander,
    pantropic_cp,
    mutant_cp,
    parasite_cp,
    breeder_cp,
    gizmo_cp,
    mole_cp,
    PROGRAM_TAGS_NUMBER
    } program_tag;

