
#include <ctype.h>

typedef enum command_opcode
    {
    cmd_nop,
    cmd_day,
    cmd_forward,
    cmd_year,
    cmd_add_dev,
    cmd_del_dev,
    cmd_add_chl,
    cmd_ext_chl,
    cmd_swap_chl,
    cmd_del_chl,
    cmd_add_mob,
    cmd_buy_dev,
    cmd_buy_mob,
    cmd_trade_dev,
    cmd_switch_dev,
    cmd_set_dev,
    cmd_set_mob,
    cmd_reset,
    cmd_start,
    cmd_load,
    cmd_save,
    cmd_dump,
    cmd_quit,
    cmd_esc,
    COMMAND_OPCODE_NUMBER
    } command_opcode;

typedef struct user_command
    {
    command_opcode opcode;
    cell *loc_arg;
    direction dir_arg;
    real num_arg;
    } user_command;

typedef enum input_mode
    {
    start_mode,
    control_mode,
    install_channel_mode,
    extend_channel_mode,
    trade_mode,
    watch_mode,
    run_mode,
    pause_mode,
    INPUT_MODE_NUMBER
    } input_mode;

#define HISTORY_LEN 10
typedef struct message_history
    {
    int last;
    message_class message[HISTORY_LEN];
    int time[HISTORY_LEN];
    boolean clear_message;
    } message_history;

typedef struct interface
    {
    input_mode mode;
    message_history history;
    cell *cursor;
    } interface;

typedef enum style_type
    {
    standard_style,
    highlight_style,
    halflight_style,
    control_style,
    install_style,
    extend_style,
    trade_style,
    failure_style,
    STYLE_TYPE_NUMBER
    } style_type;

#define MAP_ROW             0
#define LEGEND_ROW          2
#define ENV_ROW             (MAP_ROW + LEGEND_ROW + PRIMARY_RT_NUMBER + 1)
#define ENV_COL             (MAP_COLS + 3)
#define USR_ROW             (ENV_ROW + 9)
#define USR_COL             ENV_COL
#define MODE_ROW            (USR_ROW + 8)
#define MODE_COL            USR_COL
#define MESSAGE_ROW         (MAP_ROWS + 1)
#define MESSAGE_COL         0
#define ZOOM_ROW            (MESSAGE_ROW + 2)
#define LIST_ROW            ZOOM_ROW
#define HISTORY_ROW         ZOOM_ROW
#define LIST_FIRST_COL      0
#define LIST_SECOND_COL     34
#define HELP_ROW            ZOOM_ROW
#define STATS_ROW           ZOOM_ROW

/* 80-chars bar */
#define BAR "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

#define ESC_KEY             '\x1b'
#define BACKSPACE_KEY       '\x08'

#define KNOB_RESOLUTION     0.05

/* Dummy functions */
#define unlock_interface()

extern interface ifc;

/* Protos */
void open_io(void);
void close_io(void);
void init_interface(void);
user_command query_for_command(event news);
int query_for_item(const char **name, const icon_type *icon, const real *price,
    int *storage, int class_number);
void pause(void);
boolean receive_interrupt(void);
void lock_interface(void);
void display_symbol(symbol *sp);
void display_map(void);
void display_cell(cell *loc);
void display_neighbors(cell *loc);
void display_factors(void);
void display_user(void);
void display_zoom(cell *loc);
void display_ground_infos(cell *loc);
void display_device(device *plant);
void display_channel(channel *duct);
void display_mobile(mobile *turtle);
void display_frame(void);
void display_mode(input_mode mode);
void display_message(message_class message);
void display_history(void);
void display_stats(void);
void display_help(void);
void set_style(style_type style);
void redraw_screen(void);

/* Dummy protos
void unlock_interface(void);
*/

