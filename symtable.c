
#include "global.h"
#include "coredefs.h"
#include "symbols.h"

const icon_type device_icon[DEVICE_CLASS_NUMBER] =
    {
    'E',    /* Solar cell */
    'N',    /* Nuclear reactor */
    'B',    /* Battery */
    'H',    /* Heating system */
    'W',    /* Water processor */
    'U',    /* Water tank */
    'A',    /* Air processor */
    'O',    /* Air tank */
    'G',    /* Greenhouse */
    'F',    /* Food storage */
    'P',    /* Plantation */
    'S',    /* Biomass silo */
    'Y',    /* Yeaster */
    'D',    /* Distillery */
    '$',    /* Elan tank */
    'C',    /* Computer */
    '@',    /* Habitat */
    '&',    /* Hybridome */
    'M',    /* Metamorpher */
    'R'     /* Radiator */
    };

const icon_type channel_icon[DIRECTION_NUMBER] = {'|', '|', '-', '-', '\\', '/', '/', '\\'};

const color_type resource_color[RESOURCE_TYPE_NUMBER] =
    {
    electricity_color,
    heat_color,
    water_color,
    air_color,
    food_color,
    biomass_color,
    elan_color,
    COLOR_TYPE_NUMBER,          /* PRIMARY_RT_NUMBER */
    cpu_power_color,
    human_support_color,
    hybrid_support_color,
    morphing_power_color
    };

const icon_type mobile_icon[MOBILE_CLASS_NUMBER] =
    {
    'I',    /* Roboprobe */
    'J',    /* Raider */
    'K',    /* Exterminator */
    'L',    /* Stalker */
    'T',    /* Guardian */
    '_',    /* PRIMARY_MC_NUMBER */
    '&',    /* Pantropic form */
    '?',    /* Mutant form */
    'X',    /* Alien parasite */
    'Z',    /* Alien breeder */
    '#',    /* Plasma gizmo */
    '^',    /* Alien mole */
    'V'     /* Space shuttle */
    };

const color_type mobile_color[MOBILE_CLASS_NUMBER] =
    {
    robot_color,            /* Roboprobe */
    robot_color,            /* Raider */
    robot_color,            /* Exterminator */
    robot_color,            /* Stalker */
    robot_color,            /* Guardian */
    COLOR_TYPE_NUMBER,      /* PRIMARY_MC_NUMBER */
    pantropic_color,        /* Pantropic form */
    mutant_color,           /* Mutant form */
    parasite_color,         /* Alien parasite */
    breeder_color,          /* Alien breeder */
    gizmo_color,            /* Plasma gizmo */
    mole_color,             /* Alien mole */
    robot_color             /* Space shuttle */
    };

const icon_type unexplored_ground_icon[GROUND_ICON_NUMBER] =
    {
    '.',
    '.',
    ':',
    ';',
    ';'
    };

const icon_type explored_ground_icon[GROUND_ICON_NUMBER] =
    {
    ' ',
    '.',
    ':',
    ';',
    '*'
    };

