
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#include "global.h"
#include "system.h"
#include "console.h"

static struct termios c_settings;
static struct termios o_settings;

colors_pair pen_colors[PEN_NUMBER];

int open_console(const char *title_string, int x, int y, int l, int h)
{
  tcgetattr(0, &o_settings);
  memcpy(&c_settings, &o_settings, sizeof (c_settings));

  c_settings.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
			  | INLCR | IGNCR | ICRNL | IXON);
  c_settings.c_oflag |= OPOST | ONLCR;
  c_settings.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  c_settings.c_cflag &= ~(CSIZE | PARENB);
  c_settings.c_cflag |= CS8;

  tcsetattr(0, TCSAFLUSH, &c_settings);
  return 0;
}

void close_console()
{
  /*set_colors(7, 0);*/ /* CHANGE: should use default fg/bg AND the new function set_char_colors */
  clear_screen();
  tcsetattr(0, TCSAFLUSH, &o_settings);
}

char read_char()
{
    char c;

    read(0, (void *) &c, 1);
    return c;
}

char async_read_char()
{
    fd_set readfd;
    struct timeval tv;
    char c;
    cc_t t1, t2;

    t1 = c_settings.c_cc[VMIN];
    t2 = c_settings.c_cc[VTIME];
    c_settings.c_cc[VMIN] = c_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSADRAIN, &c_settings);

    FD_ZERO(&readfd);
    FD_SET(0, &readfd);
    tv.tv_sec = 0;
    tv.tv_usec = 10;

    if (!select(1, &readfd, NULL, NULL, &tv))
	c = '\0';
    else
	read(0, (void *) &c, 1);

    c_settings.c_cc[VMIN] = t1;
    c_settings.c_cc[VTIME] = t2;
    tcsetattr(0, TCSAFLUSH, &c_settings);
    return (c);
}

void con_printf(const char *format_string,...)
{
    char buffer[STRING_MAX_LEN];
    va_list arg_list;

    va_start(arg_list, format_string);
    vsprintf(buffer, format_string, arg_list);
    write_string(buffer);
}

void begin_redraw()
{
  hide_cursor();
  clear_screen();
}

void end_redraw()
{
  show_cursor();
}

int register_pen(colors_pair pair)
{
  static int last_pen;
  int pen;

  for(pen = 0; pen < last_pen; pen++)
    if(pen_colors[pen].fg == pair.fg && pen_colors[pen].bg == pair.bg)
      return pen;

  pen_colors[last_pen] = pair;
  pen_colors[reverse_pen(last_pen)].fg = pair.bg;
  pen_colors[reverse_pen(last_pen)].bg = pair.fg;

  return last_pen++;
}

void set_pen(int pen)
{
  static int prev_pen;

  if(pen != prev_pen) {
    prev_pen = pen;
    con_printf(SET_COLORS("%d", "%d"), pen_colors[pen].fg, pen_colors[pen].bg);
  }
}

#ifdef DEBUG
void locate(signed char i, signed char j)
{
  con_printf(LOCATE("%d", "%d"), i + 1, j + 1);
}

void clear_screen()
{
  write_string(CLS);
}

void clear_to_eos()
{
  write_string(ERASE2EOS);
}

void clear_to_eol()
{
  write_string(ERASE2EOL);
}

void show_cursor()
{
}

void hide_cursor()
{
}

void write_char(char output_char)
{
  write(1, &output_char, 1);
}

void write_string(const char *string)
{
  write(1, string, strlen(string));
}
#endif
