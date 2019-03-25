/*** NCurses based I/O ***/

/*
 * NOTE: this is written for ncurses, maybe it works with old BSD
 * curses, or maybe not :>
 */
#include <curses.h>


/* REVERSE_BASE is an arbitrary "big-enough" number */
#define REVERSE_BASE				64
#define PEN_NUMBER				(2 * REVERSE_BASE)

/* 
 * dummy functions 
 */
#define hide_cursor()
#define show_cursor()
#define reverse_pen(pen)		(REVERSE_BASE + (pen))
#ifndef DEBUG
#define locate(i, j) (move(i, j))
#define clear_screen() (erase())
#define clear_to_eos() (clrtobot())
#define clear_to_eol() (clrtoeol())
#define write_char(c) (addch(c))	/* Outputs control characters other
					   than tab, nl, bs as  ^X ! */
#define write_string(string) addstr(string)

#else
void locate(int i, int j);
void clear_screen(void);
void clear_to_eos(void);
void clear_to_eol(void);
void write_char(char c);
void write_string(const char *string);
#endif

#define STRING_MAX_LEN			4096	/* Usato in con_printf */

typedef struct colors_pair {
  int fg;
  int bg;
} colors_pair;

/* 
 * Protos 
 */
int open_console(const char *title_string, int x, int y, int l, int h);
void close_console(void);
char read_char(void);
char async_read_char(void);
void con_printf(const char *format_string,...);
void begin_redraw(void);
void end_redraw(void);
int register_pen(colors_pair pair);
void set_pen(int pen);
