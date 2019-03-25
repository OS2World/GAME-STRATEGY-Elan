
/*** Amiga/ANSI console I/O ***/

#include <stdio.h>
#include <stdarg.h>

/* ANSI control sequences */
#define ESC                     "\x1b"
#define CSI                     ESC "["
#define HOME                    CSI "H"
#define ERASE2EOS               CSI "J"
#define ERASE2EOL               CSI "K"
#define CLS                     HOME ERASE2EOS
#define LOCATE(i, j)            CSI i ";" j "H"
#define SET_COLORS(fg, bg)      CSI "0;3" fg ";4" bg "m"

/* Amiga control sequences */
#define SHOW_CURSOR             CSI " p"
#define HIDE_CURSOR             CSI "0 p"

#define ANSI_COLORS_NUMBER      8
#define ANSI_PAIRS_NUMBER       (ANSI_COLORS_NUMBER * ANSI_COLORS_NUMBER)
#define REVERSE_BASE            ANSI_PAIRS_NUMBER
#define PEN_NUMBER              (2 * REVERSE_BASE)

#define STRING_MAX_LEN          4096    /* Usato in con_printf */

typedef struct colors_pair
    {
    int fg;
    int bg;
    } colors_pair;

/* Dummy functions */
#define locate(i, j)            (con_printf(LOCATE("%d", "%d"), (i) + 1, (j) + 1))
#define clear_screen()          write_string(CLS)
#define clear_to_eos()          write_string(ERASE2EOS)
#define clear_to_eol()          write_string(ERASE2EOL)
#define show_cursor()           write_string(SHOW_CURSOR)
#define hide_cursor()           write_string(HIDE_CURSOR)
#define reverse_pen(pen)        (REVERSE_BASE + (pen))
#define begin_redraw()
#define end_redraw()

/* Protos */
int open_console(const char *title_string, int x, int y, int l, int h);
void close_console(void);
void write_char(char output_char);
char read_char(void);
char async_read_char(void);
void write_string(const char *string);
void con_printf(const char *format_string, ...);
int register_pen(colors_pair pair);
void set_pen(int pen);

/* Dummy protos
void locate(int i, int j);
void clear_screen(void);
void clear_2_eos(void);
void show_cursor(void);
void hide_cursor(void);
int reverse_pen(int pen);
void begin_redraw(void);
void end_redraw(void);
*/

