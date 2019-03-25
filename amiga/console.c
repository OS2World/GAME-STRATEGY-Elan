
#include "global.h"
#include "console.h"

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <proto/intuition.h>

#define WRITE_PORT_NAME		"con_output"
#define READ_PORT_NAME		"con_input"

struct Window *win;
struct NewWindow win_desc =
	{
	0, 0,		/* Start pos */
	640, 480,	/* Size */
	-1, -1,		/* Pens */
	0,			/* IDCMP flags */
	WINDOWDEPTH | WINDOWDRAG | SMART_REFRESH | ACTIVATE,	/* Flags */
	NULL,		/* Gadgets */
	NULL,		/* Checkmarks */
	NULL,		/* Title */
	NULL,		/* Custom screen */
	NULL,		/* Super-bitmap */
	0, 0,		/* Min size */
	0, 0,		/* Max size */
	WBENCHSCREEN
	};

struct IOStdReq *write_msg;
struct IOStdReq *read_msg;
struct MsgPort *write_port;
struct MsgPort *read_port;

char input_char;
boolean async_io;
colors_pair pen_colors[PEN_NUMBER];

error_class open_console(const char *title_string, int x, int y, int l, int h)
	{
	if(!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0)))
		return(NULL);

	win_desc.LeftEdge = x;
	win_desc.TopEdge = y;
	win_desc.Width = l;
	win_desc.Height = h;
	win_desc.Title = (char *)title_string;

	if(!(win = OpenWindow(&win_desc)))
		return(console_err);

	if(!(write_port = CreatePort(WRITE_PORT_NAME, 0)))
		return(console_err);

	if(!(read_port = CreatePort(READ_PORT_NAME, 0)))
		return(console_err);

	if(!(write_msg = CreateStdIO(write_port)))
		return(console_err);

	if(!(read_msg = CreateStdIO(read_port)))
		return(console_err);

	write_msg->io_Data = (APTR)win;
	write_msg->io_Length = sizeof(*win);

	if(OpenDevice("console.device", 0, (struct IORequest *)write_msg, 0))
		return(console_err);

	read_msg->io_Device = write_msg->io_Device;
	read_msg->io_Unit = write_msg->io_Unit;

	return ok;
	}

void close_console()
	{
	CloseDevice((struct IORequest *)write_msg);

	DeleteStdIO(write_msg);
	DeleteStdIO(read_msg);

	DeletePort(write_port);
	DeletePort(read_port);

	CloseWindow(win);

	CloseLibrary((struct Library *)IntuitionBase);
	}

void write_char(char output_char)
	{
	write_msg->io_Command = CMD_WRITE;
	write_msg->io_Data = &output_char;
	write_msg->io_Length = 1;

	DoIO((struct IORequest *)write_msg);
	}

char read_char()
	{
	if(!async_io)
		{
		read_msg->io_Command = CMD_READ;
		read_msg->io_Data = &input_char;
		read_msg->io_Length = 1;

		DoIO((struct IORequest *)read_msg);
		}
	else
		{
		async_io = FALSE;
		WaitIO((struct IORequest *)read_msg);
		}

	return input_char;
	}

char async_read_char()
	{
	if(!async_io)
		{
		async_io = TRUE;
		read_msg->io_Command = CMD_READ;
		read_msg->io_Data = &input_char;
		read_msg->io_Length = 1;

		SendIO((struct IORequest *)read_msg);
		}

	if(CheckIO((struct IORequest *)read_msg))
		{
		async_io = FALSE;
		return input_char;
		}
	else
		return '\0';
	}

void write_string(const char *string)
	{
	write_msg->io_Command = CMD_WRITE;
	write_msg->io_Data = (char *)string;
	write_msg->io_Length = -1;

	DoIO((struct IORequest *)write_msg);
	}

void con_printf(const char *format_string, ...)
	{
	char buffer[STRING_MAX_LEN];
	va_list arg_list;

	va_start(arg_list, format_string);
	vsprintf(buffer, format_string, arg_list);
	write_string(buffer);
	va_end(arg_list);
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

	if(pen != prev_pen)
		{
		prev_pen = pen;
		con_printf(SET_COLORS("%d", "%d"), pen_colors[pen].fg, pen_colors[pen].bg);
		}
	}

