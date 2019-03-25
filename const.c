
#include "global.h"
#include "coredefs.h"
#include "params.h"

const real resource_unit_price[PRIMARY_RT_NUMBER] =
    {
    0.01,
    0,
    0.06,
    0.09,
    0.35,
    0.20,
    1
    };

const real channel_unit_price[PRIMARY_RT_NUMBER] =
    {
    1,
    6,
    4,
    5,
    9,
    11,
    18
    };

const real device_price[DEVICE_CLASS_NUMBER] =
    {
     120,   /* Solar cell */
    3990,   /* Nuclear reactor */
     170,   /* Battery */
     280,   /* Heating system */
     690,   /* Water processor */
     180,   /* Water tank */
     810,   /* Air processor */
     200,   /* Air tank */
    1100,   /* Greenhouse */
     150,   /* Food storage */
     220,   /* Plantation */
     330,   /* Biomass silo */
    7750,   /* Yeaster */
    3500,   /* Distillery */
     960,   /* Elan tank */
    2400,   /* Computer */
    1950,   /* Habitat */
    2890,   /* Hybridome */
    4370,   /* Metamorpher */
      95    /* Radiator */
    };

const real mobile_price[PRIMARY_MC_NUMBER] =
    {
     880,   /* Roboprobe */
    1230,   /* Raider */
    3790,   /* Exterminator */
    3950,   /* Stalker */
    9990    /* Guardian */
    };

const int mobile_max_age[MOBILE_CLASS_NUMBER] =
    {
    0,                          /* Roboprobe */
    0,                          /* Raider */
    0,                          /* Exterminator */
    0,                          /* Stalker */
    0,                          /* Guardian */
    0,                          /* PRIMARY_MC_NUMBER */
    (10 * 2 * BASE_PERIOD),     /* Pantropic form */
    (15 * 2 * BASE_PERIOD),     /* Mutant form */
    (7 * 2 * BASE_PERIOD),      /* Alien parasite */
    (21 * 2 * BASE_PERIOD),     /* Alien breeder */
    (2 * MIN_TIME_GRANTED),     /* Plasma gizmo */
    0,                          /* Alien mole */
    SHUTTLE_STAY                /* Space shuttle */
    };

/* *INDENT-OFF* */
const real csi[MOBILE_CLASS_NUMBER][MOBILE_CLASS_NUMBER] =
    {/*  Rbp  Rdr  Ext  Stk  Gua   #   Pan  Mut APar ABre  Giz AMol SShu <-Def    Atk */
    {     0,   0,   0,   0,   0,   0,   3,   3,   2,   6,   0,   0,   0   },   /* Roboprobe */
    {     0,   0,   0,   0,   0,   0,   0,   0,   1,   3,   0,   0,   0   },   /* Raider */
    {     0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   0,   0,   0   },   /* Exterminator */
    {     0,   0,   0,   0,   0,   0,   0,   0,   1,   3,   1,  33,   0   },   /* Stalker */
    {     3,   0,   0,   0,   0,   0,   0,   1,   1,   2,   1,  11,   0   },   /* Guardian */
    {     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0   },   /* PRIMARY_MC_# */
    {     0,   0,   0,   0,   0,   0,   0,   4,   3,   9,   0,   0,   0   },   /* Pantropic */
    {    12,  24,  48,  72,  96,   0,   4,   4,   3,   9,   0,   0,   0   },   /* Mutant */
    {    15,  30,  60,  90, 120,   0,   5,   5,   0,   0,   0,   0,   0   },   /* Parasite */
    {     3,   6,  12,  18,  24,   0,   1,   1,   0,   0,   0,   0,   0   },   /* Breeder */
    {     1,   3,   9,  81,  81,   0,   0,   0,   0,   0,   0,   0,   0   },   /* Gizmo */
    {     0,   0,   0,   0,   0,   0,   3,   3,   0,   0,   0,   0,   0   },   /* Mole */
    {     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0   }    /* Shuttle */
    };
/* *INDENT-ON* */

