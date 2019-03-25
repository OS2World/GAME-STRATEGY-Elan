
/*** ANSI console I/O for Unix systems ***/

#include <stdio.h>
#include <stdarg.h>

#define ANSI_COLORS_NUMBER		16
#define ANSI_PAIRS_NUMBER		(ANSI_COLORS_NUMBER * ANSI_COLORS_NUMBER)
#define REVERSE_BASE			ANSI_PAIRS_NUMBER
#define PEN_NUMBER				(2 * REVERSE_BASE)

#define reverse_pen(pen)		(REVERSE_BASE + (pen))
void locate(signed char i, signed char j);
void clear_screen(void);
void clear_to_eos(void);
void clear_to_eol(void);
void show_cursor(void);
void hide_cursor(void);
void begin_redraw(void);
void end_redraw(void);
void write_char(char output_char);
void write_string(const char *string);

#define STRING_MAX_LEN			4096	/* Usato in con_printf */

typedef struct colors_pair
{
	int fg;
	int bg;
} colors_pair;

/* Protos */
int open_console(const char *title_string, int x, int y, int l, int h);
void close_console(void);
char read_char(void);
char async_read_char(void);
void con_printf(const char *format_string, ...);
int register_pen(colors_pair pair);
void set_pen(int pen);

