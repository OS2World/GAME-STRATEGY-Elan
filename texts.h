
typedef enum message_class
    {
    blank_message,
    title_message,
    shuttle_arrived,
    shuttle_departed,
    system_failure,
    system_attacked,
    channel_attacked,
    system_repaired,
    channel_repaired,
    human_support_critical,
    hybrid_support_critical,
    human_birth,
    hybrid_morphed,
    pantropic_birth,
    mutant_birth,
    robot_terminated,
    pantropic_terminated,
    mutant_terminated,
    alien_terminated,
    colony_terminated,
    select_item_to_buy,
    insuff_money_to_buy_item,
    select_item_to_install,
    item_not_available,
    square_not_empty,
    square_not_explored,
    neighbor_plant,
    too_many_devices,
    too_many_channels,
    too_many_mobiles,
    resource_not_exportable,
    insuff_money_to_build_channel,
    impossible_to_cross_square,
    game_loaded,
    file_mismatch,
    load_error,
    game_saved,
    save_error,
    place_habitat,
    confirm_departure,
    ask_for_help,
    end_of_game,
    MESSAGE_CLASS_NUMBER
    } message_class;

#define QUALITIES_NUMBER    8
#define SEASONS_NUMBER      4

extern const char *quality[QUALITIES_NUMBER];
extern const char *season_name[SEASONS_NUMBER];
extern const char *device_name[DEVICE_CLASS_NUMBER];
extern const char *resource_name[RESOURCE_TYPE_NUMBER];
extern const char *measure_unit[RESOURCE_TYPE_NUMBER];
extern const char *mobile_name[MOBILE_CLASS_NUMBER];
extern const char *program_tag_name[PRIMARY_PT_NUMBER];
extern const char *message_text[MESSAGE_CLASS_NUMBER];

