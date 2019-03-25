
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "global.h"
#include "system.h"
#include "console.h"


int pen_colors[PEN_NUMBER];
typedef struct {
  int idx;
  colors_pair pair;
} colors_triple;
colors_triple triples[PEN_NUMBER];
int n_triples;

static int cmp_triple(const void *a, const void *b)
{
  colors_triple *a1 = (colors_triple *) a;
  colors_triple *b1 = (colors_triple *) b;

  if (a1->pair.fg > b1->pair.fg)
    return 1;
  else if (a1->pair.fg < b1->pair.fg)
    return -1;
  else if (a1->pair.bg > b1->pair.bg)
    return 1;
  else if (a1->pair.bg < b1->pair.bg)
    return -1;
  else
    return 0;
  /*return ((a1->pair.fg - b1->pair.fg) ? 
	  (a1->pair.fg - b1->pair.fg) : (a1->pair.bg - b1->pair.bg));*/
}

int
open_console(const char *title_string, int x, int y, int l, int h)
{
  int i, n;

  initscr();
  start_color();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  /*keypad(stdscr, TRUE);*/

  if (!has_colors()) {
    close_console();
    fprintf(stderr,
	    "Elan needs a color terminal. Sorry.\n");
    exit(1);
  }
  if (LINES < 45 || COLS < 80) { /* CHANGE: le dim. dovrebbero essere un po' meno hardwired! */
    close_console();
    fprintf(stderr, 
	    "Elan needs a terminal 80 chars wide * 45 lines high at least, sorry.\n");
    exit(1);
  }

  /* Now we minimize the # of color pairs */
  qsort(triples, n_triples * 2, sizeof (colors_triple), cmp_triple);
  n = 1;
  init_pair(n, triples[0].pair.fg, triples[0].pair.bg);
  pen_colors[triples[0].idx] = 1;
  for (i = 1; i < n_triples * 2; i++) {
    if (triples[i - 1].pair.fg == triples[i].pair.fg
	&& triples[i - 1].pair.bg == triples[i].pair.bg) {
      pen_colors[triples[i].idx] = pen_colors[triples[i - 1].idx];
      continue;
    }
    if (++n >= COLOR_PAIRS) {
      close_console();
      fprintf(stderr,
	      "This terminal supports too few colors.\n"
	      "Maybe you can change Elan configuration file to reduce the number of colors?\n");
      exit(1);
    }
    init_pair(n, triples[i].pair.fg, triples[i].pair.bg);
    pen_colors[triples[i].idx] = n;
  }

  return 0;
}

void
close_console()
{
  free(triples);
  endwin();
}

char
read_char()
{
  refresh();
  nodelay(stdscr, FALSE);
  return (getch());
}

char
async_read_char()
{
  char c;

  refresh();
  nodelay(stdscr, TRUE);
  c = getch();
  return ((c == ERR) ? 0 : c);
}

void
con_printf(const char *format_string,...)
{
  char buffer[STRING_MAX_LEN];
  va_list arg_list;

  va_start(arg_list, format_string);
  vsprintf(buffer, format_string, arg_list);
  write_string(buffer);
}

int register_pen(colors_pair pair)
{
  if (n_triples >= PEN_NUMBER) {
    fprintf(stderr, "INTERNAL ERROR: trying to register too many colors!\n");
    exit(1);
  }
  triples[n_triples * 2].idx = n_triples;
  triples[n_triples * 2].pair = pair;
  triples[n_triples * 2 + 1].idx = reverse_pen(n_triples);
  triples[n_triples * 2 + 1].pair.fg = pair.bg;
  triples[n_triples * 2 + 1].pair.bg = pair.fg;

  return n_triples++;
}

void set_pen(int pen)
{
  attron(COLOR_PAIR(pen_colors[pen]));
}


#ifdef DEBUG
void
locate(int i, int j)
{
  move(i, j);
}

void
clear_screen()
{
  erase();
}

void
clear_to_eos()
{
  clrtobot();
}

void
clear_to_eol()
{
  clrtoeol();
}

void
begin_redraw()
{
  clear();
}

void
end_redraw()
{
  touchwin(stdscr);
  refresh();
  redrawwin(stdscr);
}

void
write_char(char c)
{
  addch(c);			/* Outputs control characters other than tab,
				   nl or bs as ^X ! */
}

void
write_string(const char *string)
{
  addstr(string);
}
#endif
