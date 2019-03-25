
/*** ANSI console I/O for Unix systems ***/

#include <stdio.h>
#include <stdarg.h>

/* Control sequences */
#define ESC						"\x1b"
#define CSI						ESC "["
#define HOME					CSI "H"
#define ERASE2EOS				CSI "J"
#define ERASE2EOL				CSI "K"
#define CLS						HOME ERASE2EOS
#define LOCATE(i, j)			CSI i ";" j "H"
#define SET_COLORS(fore, back)	CSI "0;3" fore ";4" back "m"

/* These work only on the Amiga? */
/*#define SHOW_CURSOR                           CSI " p" */
/*#define HIDE_CURSOR                           CSI "0 p" */

#define ANSI_COLORS_NUMBER		8
#define ANSI_PAIRS_NUMBER		(ANSI_COLORS_NUMBER * ANSI_COLORS_NUMBER)
#define REVERSE_BASE			ANSI_PAIRS_NUMBER
#define PEN_NUMBER				(2 * REVERSE_BASE)

/* Dummy functions */
#define reverse_pen(pen)		(REVERSE_BASE + (pen))
#ifndef DEBUG
#define locate(i, j)			(con_printf(LOCATE("%d", "%d"), (i) + 1, (j) + 1))
#define	clear_screen()			write_string(CLS)
#define clear_to_eos()			write_string(ERASE2EOS)
#define clear_to_eol()			write_string(ERASE2EOL)
#define show_cursor()			/*write_string(SHOW_CURSOR)*/
#define hide_cursor()			/*write_string(HIDE_CURSOR)*/
#define write_char() (write(1, &output_char, 1))
#define write_string()  (write(1, string, strlen(string)))
#else
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
#endif

#define STRING_MAX_LEN			4096	/* Usato in con_printf */

typedef struct colors_pair {
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
