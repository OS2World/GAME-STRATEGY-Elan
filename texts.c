
#include "global.h"
#include "coredefs.h"
#include "texts.h"

const char *season_name[SEASONS_NUMBER] =
    {
    "Spring",
    "Summer",
    "Autumn",
    "Winter"
    };

const char *quality[QUALITIES_NUMBER] =
    {
    "Terrible",
    "Very Bad",
    "Bad",
    "Poor",
    "Fair",
    "Good",
    "Very Good",
    "Excellent"
    };

const char *device_name[DEVICE_CLASS_NUMBER] =
    {
    "Solar cell",
    "Nuclear reactor",
    "Battery",
    "Heating system",
    "Water processor",
    "Water tank",
    "Air processor",
    "Air tank",
    "Greenhouse",
    "Food storage",
    "Plantation",
    "Biomass silo",
    "Yeaster",
    "Distillery",
    "Elan tank",
    "Computer",
    "Habitat",
    "Hybridome",
    "Metamorpher",
    "Radiator"
    };

const char *resource_name[RESOURCE_TYPE_NUMBER] =
    {
    "Electricity",
    "Heat",
    "Water",
    "Air",
    "Food",
    "Biomass",
    "Elan",
    "",             /* PRIMARY_RT_NUMBER */
    "CPU power",
    "Life support",
    "Life support",
    "Morphing power"
    };

const char *measure_unit[RESOURCE_TYPE_NUMBER] =
    {
    "Kwh",
    "Kcal",
    "l",
    "l",
    "Kg",
    "Kg",
    "ml",
    "",             /* PRIMARY_RT_NUMBER */
    "Horner",
    "units",
    "units",
    "units"
    };

const char *mobile_name[MOBILE_CLASS_NUMBER] =
    {
    "Roboprobe",
    "Raider",
    "Exterminator",
    "Stalker",
    "Guardian",
    "",                 /* PRIMARY_MC_NUMBER */
    "Pantropic form",
    "Mutant form",
    "Alien parasite",
    "Alien breeder",
    "Plasma gizmo",
    "Alien mole",
    "Space shuttle"
    };

const char *program_tag_name[PRIMARY_PT_NUMBER] =
    {
    "Survey",
    "Explore",
    "Hunt",
    "Patrol"
    };

const char *message_text[MESSAGE_CLASS_NUMBER] =
    {
    "",
    TITLE,
    "Shuttle arrival",
    "Shuttle departure",
    "Plant failure",
    "Plant under attack",
    "Duct under attack",
    "Plant repaired",
    "Duct repaired",
    "Habitat life support critical",
    "Hybridome life support critical",
    "Human cloned",
    "Hybrid metamorphed",
    "Pantropic form generated",
    "Mutant form generated",
    "Robot destroyed",
    "Pantropic form terminated",
    "Mutant form terminated",
    "Alien form terminated",
    "End of colony",
    "Select item to buy",
    "Not enough money to buy item",
    "Select item to install",
    "Item not available",
    "Square not empty",
    "Robot must be placed in monitored area",
    "Plant cannot be installed near other plants",
    "Too many plants",
    "Too many ducts",
    "Too many lifeforms",
    "Resource not exportable",
    "Not enough money to build duct",
    "Duct can't cross the obstacle",
    "Game loaded",
    "Game loaded -- version mismatch",
    "Load error",
    "Game saved",
    "Save error",
    "Place habitat [0], rescan [Space], load [L], save [S] or exit [ESC]",
    "Press [Space] or [Tab] to leave trade mode",
    "Press [?] for help",
    "Current score is final. Press any key to exit."
    };

