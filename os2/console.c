#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#define INCL_DOSERRORS
#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "system.h"
#include "console.h"
#include "coredefs.h"

#define CON_COLS	(MAP_COLS + 30)
#define CON_ROWS	(MAP_ROWS + 20)

void EXPENTRY windowthread(ULONG arg);
MRESULT EXPENTRY ElanProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
void paintch(void);
void makerect(RECTL *r, short r1, short c1, short r2, short c2);
void makepos(POINTL *p, short r, short c);
void rect2path(HPS hps, RECTL *r);

static const long colormap[] = { CLR_BLACK, CLR_BLUE, CLR_RED, CLR_PINK,
	   CLR_GREEN, CLR_CYAN, CLR_YELLOW, CLR_WHITE, CLR_DARKGRAY,
	   CLR_DARKBLUE, CLR_DARKRED, CLR_DARKPINK, CLR_DARKGREEN,
	   CLR_DARKCYAN, CLR_BROWN, CLR_PALEGRAY };
colors_pair pen_colors[PEN_NUMBER];
char		screentext[CON_ROWS * CON_COLS];
int 		screencolor[CON_ROWS * CON_COLS];
HEV 		initializationsem;
int 		initresult;
HWND		elanhwnd;
HWND		framehwnd;
TID 		thr;
HAB 		elanhab;
HMQ 		elanhmq;
int 		curr_pen = 0;
signed char row;
signed char col;
HQUEUE		kbq;
short		cursorvisible;
short		cursorenabled;
HPS         hps;
HMTX		psaccess;
short		textoffs;
short       sizex;
short		sizey;

int open_console(const char *title_string, int x, int y, int l, int h)
{
	elanhab = WinInitialize(0L);
	elanhmq = WinCreateMsgQueue(elanhab, 0L);

    memset(screentext, ' ', sizeof(screentext));
	memset(screencolor, 0, sizeof(screencolor));
	if(DosCreateEventSem(0, &initializationsem, 0, FALSE) == 0 &&
		DosCreateMutexSem(0, &psaccess, 0, FALSE) == 0 &&
		DosCreateQueue(&kbq, 0, "\\QUEUES\\ELAN\\KBQUEUE") == 0 &&
		DosCreateThread(&thr, windowthread, (ULONG)title_string, 0, 65536) == 0 &&
		DosWaitEventSem(initializationsem, SEM_INDEFINITE_WAIT) == 0 &&
		initresult == 0)
	{
		DosCloseEventSem(initializationsem);
	}
	else
	{
		initresult = 1;
		WinDestroyMsgQueue(elanhmq);
		WinTerminate(elanhab);
	}

	return initresult;
}

void EXPENTRY windowthread(ULONG arg)
{
	HAB 	hab;
	HMQ 	hmq;
	QMSG	qmsg;
	ULONG	fcf = FCF_TITLEBAR | FCF_SYSMENU | FCF_MINBUTTON |
			FCF_BORDER | FCF_TASKLIST | FCF_ICON;

	initresult = 0;

	hab = WinInitialize(0L);
	hmq = WinCreateMsgQueue(hab, 0L);

	WinRegisterClass(hab, "Elan", ElanProc, CS_SYNCPAINT, 0L);

	if((framehwnd = WinCreateStdWindow(HWND_DESKTOP, WS_ANIMATE,
			&fcf, "Elan", (char *)arg, WS_VISIBLE, 0L, 1L, &elanhwnd)) == 0)
		initresult = 1;

	DosPostEventSem(initializationsem);

	if(initresult == 0)
	{
		/* Event loop
		 */
		while(WinGetMsg(hab, &qmsg, 0, 0, 0))
			WinDispatchMsg(hab, &qmsg);
	}

	WinDestroyWindow(framehwnd);

	WinDestroyMsgQueue(hmq);
	WinTerminate(hab);
}

MRESULT EXPENTRY ElanProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	static HAB		hab;
	static LONG 	incr[CON_COLS + 1];
	MRESULT 	ret = (MRESULT)FALSE;
	SIZEL		size;
	RECTL		rec;
	RECTL		back;
	HDC 		hdc;
	POINTL		pos;
	FATTRS		font;
	short		wndx;
	short		wndy;
	LONG		len;
	LONG		run;
	LONG		offs;
	SIZEF		box;
	FONTMETRICS fm;
	short		y;
	short		i;
	short		tmp;
	FONTMETRICS *afm = NULL;
	LONG		count = 0;
	short		xlimit;
	short		ylimit;
	short		dist;
	short		mindist;
	short		found;

	switch(msg)
	{
		case WM_CREATE :
            ret = (MRESULT)TRUE;

            size.cx = 0;
            size.cy = 0;

			hab = WinQueryAnchorBlock(hwnd);
			hdc = WinOpenWindowDC(hwnd);
			hps = GpiCreatePS(hab, hdc, &size, PU_PELS | GPIT_MICRO | GPIA_ASSOC);

			if(hps != 0)
			{
                memset(&font, 0, sizeof(font));
                font.usRecordLength = sizeof(font);

				count = GpiQueryFonts(hps, QF_PUBLIC, "System VIO", &count,
					sizeof(*afm), (PFONTMETRICS)NULL);

				if((afm = malloc(sizeof(*afm) * count)) != 0)
				{
					GpiQueryFonts(hps, QF_PUBLIC, "System VIO",
						&count, sizeof(*afm), afm);

					xlimit = (0.75 * WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN)) / CON_COLS;
					ylimit = (0.75 * (WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN) - WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR))) / CON_ROWS;

					found = -1;
					for(i = 0; i < count; i++)
					{
						if(! (afm[i].fsDefn & FM_DEFN_OUTLINE) &&
							afm[i].lMaxBaselineExt <= ylimit &&
							afm[i].lAveCharWidth <= xlimit)
						{
							dist = (ylimit - afm[i].lMaxBaselineExt) + (xlimit - afm[i].lAveCharWidth);
							if(found == -1 || dist < mindist)
							{
								mindist = dist;
								found = i;
							}
						}
					}

					if(found != -1)
					{
						strcpy(font.szFacename, afm[found].szFacename);
						font.idRegistry = afm[found].idRegistry;
						font.lMaxBaselineExt = afm[found].lMaxBaselineExt;
						font.lAveCharWidth = afm[found].lAveCharWidth;

						GpiCreateLogFont(hps, 0, 42, &font);
						GpiSetCharSet(hps, 42);

						GpiQueryFontMetrics(hps, sizeof(fm), &fm);
						sizex = fm.lMaxCharInc;
						sizey = fm.lEmHeight * 1.3 + fm.lExternalLeading;
						textoffs = fm.lMaxDescender + (sizey - fm.lEmHeight - fm.lExternalLeading) / 2;

						for(i = 0; i < CON_COLS + 1; i++)
							incr[i] = sizex;

						wndx = 2 * WinQuerySysValue(HWND_DESKTOP, SV_CXBORDER) +
								CON_COLS * sizex;
						wndy = WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR) +
								2 * WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER) +
								CON_ROWS * sizey;

						WinSetWindowPos(WinQueryWindow(hwnd, QW_PARENT), 0, 10, 10,
							wndx, wndy, SWP_MOVE | SWP_SIZE | SWP_SHOW | SWP_ACTIVATE);

						WinStartTimer(hab, hwnd, 100, WinQuerySysValue(HWND_DESKTOP, SV_CURSORRATE));
						cursorvisible = 0;
						cursorenabled = 0;

						ret = (MRESULT)FALSE;
                    }
                    free(afm);
                }
			}
			break;

		case WM_DESTROY :
			if(DosWriteQueue(kbq, 27, 0, 0, 0) == 0 &&
				DosWriteQueue(kbq, 27, 0, 0, 0) == 0)
                GpiDestroyPS(hps);
			else
				DosBeep(880, 100);
            break;

		case WM_CHAR :
			if(! (SHORT1FROMMP(mp1) & KC_KEYUP) && SHORT1FROMMP(mp2) != 0)
				if(DosWriteQueue(kbq, SHORT1FROMMP(mp2), 0, 0, 0) != 0)
					DosBeep(880, 100);
			ret = (MRESULT)TRUE;	/* eat event */
			break;

        case WM_PAINT :
			DosRequestMutexSem(psaccess, SEM_INDEFINITE_WAIT);
			WinBeginPaint(hwnd, hps, &rec);
			GpiSetColor(hps, CLR_BLACK);
			GpiSetBackColor(hps, CLR_BLACK);
			GpiSetMix(hps, FM_OVERPAINT);
			GpiBeginPath(hps, 1L);
			rect2path(hps, &rec);
			GpiEndPath(hps);
			GpiFillPath(hps, 1L, FPATH_ALTERNATE);

			for(y = 0; y < CON_ROWS; y++)
			{
				pos.x = 0;
				pos.y = sizey * (CON_ROWS - 1 - y) + textoffs;

				offs = CON_COLS * y;
				run = 0;
				do
				{
					len = 1;
					while(run + len < CON_COLS && screencolor[offs + len] == screencolor[offs])
						len++;

					GpiSetMix(hps, FM_OVERPAINT);
					GpiSetColor(hps, colormap[pen_colors[screencolor[offs]].fg]);
					GpiSetBackColor(hps, colormap[pen_colors[screencolor[offs]].bg]);
					makerect(&back, y, run, y, run + len);
					GpiCharStringPosAt(hps, &pos, &back, CHS_VECTOR | CHS_OPAQUE,
						len, screentext + offs, incr);

                    pos.x += len * sizex;
                    offs += len;
					run += len;
				}
				while(run < CON_COLS);
				pos.y -= sizey;
			}
			WinEndPaint(hps);
			DosReleaseMutexSem(psaccess);
            ret = (MRESULT)FALSE;
            break;

		case WM_TIMER :
			if(cursorenabled && DosRequestMutexSem(psaccess, SEM_IMMEDIATE_RETURN) == 0)
			{
				makerect(&rec, row, col, row, col);
				GpiSetColor(hps, CLR_WHITE);
				GpiSetBackColor(hps, CLR_BLACK);
				GpiSetMix(hps, FM_XOR);
				GpiBeginPath(hps, 1L);
				rect2path(hps, &rec);
				GpiEndPath(hps);
				GpiFillPath(hps, 1L, FPATH_ALTERNATE);
				cursorvisible = 1 - cursorvisible;
				DosReleaseMutexSem(psaccess);
			}
			break;

		default :
			ret = WinDefWindowProc(hwnd, msg, mp1, mp2);
			break;
	}

	return ret;
}

void close_console()
{
	WinPostMsg(elanhwnd, WM_QUIT, MPVOID, MPVOID);
	DosWaitThread(&thr, DCWW_WAIT);

	DosCloseMutexSem(psaccess);
	DosCloseQueue(kbq);

    WinDestroyMsgQueue(elanhmq);
	WinTerminate(elanhab);
}

char read_char()
{
	REQUESTDATA req;
	void		*data;
	ULONG		size;
	BYTE		pri;

	DosReadQueue(kbq, &req, &size, &data, 0, DCWW_WAIT, &pri, 0);

	return req.ulData;
}

char async_read_char()
{
	REQUESTDATA req;
	void		*data;
	ULONG		size;
    BYTE        pri;
	char		c;

	if(DosReadQueue(kbq, &req, &size, &data, 0, DCWW_NOWAIT, &pri, 0) == ERROR_QUE_EMPTY)
		c = 0;
	else
		c = req.ulData;

	return c;
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
	static int last_pen = 0;
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
	curr_pen = pen;
}

void locate(signed char i, signed char j)
{
	RECTL	rec;

	if(cursorvisible)
	{
		DosRequestMutexSem(psaccess, SEM_INDEFINITE_WAIT);
		makerect(&rec, row, col, row, col);
		GpiSetColor(hps, CLR_WHITE);
		GpiSetBackColor(hps, CLR_BLACK);
		GpiSetMix(hps, FM_XOR);
		GpiBeginPath(hps, 1L);
		rect2path(hps, &rec);
		GpiEndPath(hps);
		GpiFillPath(hps, 1L, FPATH_ALTERNATE);
        cursorvisible = 0;
		DosReleaseMutexSem(psaccess);
    }
    row = i;
	col = j;
}

void clear_screen()
{
	memset(screentext, ' ', sizeof(screentext));
	memset(screencolor, 0, sizeof(screencolor));
}

void clear_to_eos()
{
	RECTL	rec;
    short   pos = CON_COLS * row + col;

	memset(screentext + pos, ' ', sizeof(screentext) - pos * sizeof(screentext[0]));
	memset(screencolor + pos, 0, sizeof(screencolor) - pos * sizeof(screencolor[0]));

	DosRequestMutexSem(psaccess, SEM_INDEFINITE_WAIT);

	makerect(&rec, row, col, row, CON_COLS - 1);
	GpiSetColor(hps, CLR_BLACK);
	GpiSetBackColor(hps, CLR_BLACK);
	GpiSetMix(hps, FM_OVERPAINT);
	GpiBeginPath(hps, 1L);
	rect2path(hps, &rec);
	GpiEndPath(hps);
	GpiFillPath(hps, 1L, FPATH_ALTERNATE);

	makerect(&rec, row + 1, 0, CON_ROWS - 1, CON_COLS - 1);
	GpiSetColor(hps, CLR_BLACK);
	GpiSetBackColor(hps, CLR_BLACK);
	GpiSetMix(hps, FM_OVERPAINT);
	GpiBeginPath(hps, 1L);
	rect2path(hps, &rec);
	GpiEndPath(hps);
	GpiFillPath(hps, 1L, FPATH_ALTERNATE);

	DosReleaseMutexSem(psaccess);
}

void clear_to_eol()
{
	RECTL	rec;
	short	pos = CON_COLS * row + col;
	short	size = CON_COLS - col;

	memset(screentext + pos, ' ', size * sizeof(screentext[0]));
	memset(screencolor + pos, 0, size * sizeof(screencolor[0]));

	DosRequestMutexSem(psaccess, SEM_INDEFINITE_WAIT);

	makerect(&rec, row, col, row, CON_COLS - 1);
	GpiSetColor(hps, CLR_BLACK);
	GpiSetBackColor(hps, CLR_BLACK);
	GpiSetMix(hps, FM_OVERPAINT);
	GpiBeginPath(hps, 1L);
	rect2path(hps, &rec);
	GpiEndPath(hps);
	GpiFillPath(hps, 1L, FPATH_ALTERNATE);

	DosReleaseMutexSem(psaccess);
}

void show_cursor()
{
	cursorenabled = 1;
}

void hide_cursor()
{
	RECTL	rec;

	cursorenabled = 0;
	if(cursorvisible)
	{
		DosRequestMutexSem(psaccess, SEM_INDEFINITE_WAIT);
		makerect(&rec, row, col, row, col);
		GpiSetColor(hps, CLR_WHITE);
		GpiSetBackColor(hps, CLR_BLACK);
		GpiSetMix(hps, FM_XOR);
		GpiBeginPath(hps, 1L);
		rect2path(hps, &rec);
		GpiEndPath(hps);
		GpiFillPath(hps, 1L, FPATH_ALTERNATE);
		cursorvisible = 0;
		DosReleaseMutexSem(psaccess);
    }
}

void write_char(char output_char)
{
	if(output_char == 0x0a)
	{
		col = 0;
		row++;
		if(row == CON_ROWS)
			row = 0;
	}
	else
	{
		screentext[CON_COLS * row + col] = output_char;
		screencolor[CON_COLS * row + col] = curr_pen;
		paintch();
        col++;
		if(col == CON_COLS)
		{
			col = 0;
			row++;
			if(row == CON_ROWS)
				row = 0;
		}
	}
}

void write_string(const char *string)
{
	while(*string)
	{
		if(*string == 0x0a)
		{
			col = 0;
			row++;
			if(row == CON_ROWS)
				row = 0;
		}
		else
		{
			screentext[CON_COLS * row + col] = *string;
			screencolor[CON_COLS * row + col] = curr_pen;
			paintch();
			col++;
			if(col == CON_COLS)
			{
				col = 0;
				row++;
				if(row == CON_ROWS)
					row = 0;
			}
		}
		string++;
	}
}

void paintch(void)
{
	short	offs = CON_COLS * row + col;
    POINTL  pos;
	RECTL	back;

	DosRequestMutexSem(psaccess, SEM_INDEFINITE_WAIT);

	GpiSetMix(hps, FM_OVERPAINT);
	GpiSetColor(hps, colormap[pen_colors[screencolor[offs]].fg]);
	GpiSetBackColor(hps, colormap[pen_colors[screencolor[offs]].bg]);
	makepos(&pos, row, col);
	makerect(&back, row, col, row, col);
	GpiCharStringPosAt(hps, &pos, &back, CHS_OPAQUE, 1, screentext + offs, 0);

	DosReleaseMutexSem(psaccess);
}

void makerect(RECTL *r, short r1, short c1, short r2, short c2)
{
	short	t;
	if(r2 < r1)
		t = r1, r1 = r2, r2 = t;
	if(c2 < c1)
		t = c1, c1 = c2, c2 = t;

	r->xLeft = sizex * c1;
	r->xRight = sizex * c2 + sizex - 1;
	r->yTop = sizey * (CON_ROWS - 1 - r1) + sizey - 1;
	r->yBottom = sizey * (CON_ROWS - 1 - r2);
}

void makepos(POINTL *p, short r, short c)
{
	p->x = sizex * c;
	p->y = sizey * (CON_ROWS - 1 - r) + textoffs;
}

void rect2path(HPS hps, RECTL *r)
{
	POINTL	pos;

	pos.x = r->xLeft;
	pos.y = r->yBottom;
	GpiMove(hps, &pos);
	pos.x = r->xRight;
	pos.y = r->yBottom;
	GpiLine(hps, &pos);
	pos.x = r->xRight;
	pos.y = r->yTop;
	GpiLine(hps, &pos);
	pos.x = r->xLeft;
	pos.y = r->yTop;
	GpiLine(hps, &pos);
	pos.x = r->xLeft;
	pos.y = r->yBottom;
	GpiLine(hps, &pos);
}

