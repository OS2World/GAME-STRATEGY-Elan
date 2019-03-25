
typedef int icon_type;      /* Tipi di rappresentazione interna */

typedef enum color_type
    {
    unexplored_square_color,
    explored_square_color,
    electricity_color,
    heat_color,
    water_color,
    air_color,
    food_color,
    biomass_color,
    elan_color,
    cpu_power_color,
    human_support_color,
    hybrid_support_color,
    morphing_power_color,
    robot_color,
    pantropic_color,
    mutant_color,
    parasite_color,
    breeder_color,
    gizmo_color,
    mole_color,
    COLOR_TYPE_NUMBER
    } color_type;

typedef enum attribute_type
    {
    plain,
    idle_device,
    active_device,
    empty_device,
    full_device,
    broken_device,
    broken_channel,
    idle_mobile,
    active_mobile,
    gravid_mobile,
    hit_mobile,
    ATTRIBUTE_TYPE_NUMBER
    } attribute_type;

typedef struct symbol
    {
    icon_type icon;
    color_type color;
    attribute_type attribute;
    boolean updated;
    } symbol;

#define GROUND_ICON_NUMBER 5

#define CORNER_ICON     '+'
#define CROSS_ICON      'x'
#define LINK_ICON(n)    ('0' + (n))
#define BREAK_ICON      '%'
#define BLOW_ICON       '#'

extern const icon_type device_icon[DEVICE_CLASS_NUMBER];
extern const icon_type channel_icon[DIRECTION_NUMBER];
extern const icon_type mobile_icon[MOBILE_CLASS_NUMBER];
extern const icon_type unexplored_ground_icon[GROUND_ICON_NUMBER];
extern const icon_type explored_ground_icon[GROUND_ICON_NUMBER];
extern const color_type resource_color[RESOURCE_TYPE_NUMBER];
extern const color_type mobile_color[MOBILE_CLASS_NUMBER];

/* Protos */
void set_icon(symbol *sp, icon_type icon);
void set_color(symbol *sp, color_type color);
void set_attribute(symbol *sp, attribute_type attribute);

