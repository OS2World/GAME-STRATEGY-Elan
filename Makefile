#DFLAGS=-O4 -fomit-frame-pointer -funroll-loops 
DFLAGS = -g -DDEBUG

CFLAGS=-pipe -Wall -Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith -Wwrite-strings -I/usr/include/ncurses ${DFLAGS}
LOADLIBES=-lm -lncurses
LDFLAGS=${DFLAGS}

INDENTFLAGS=--tab-size8 \
            --line-length78 \
            --leave-preprocessor-space \
            --format-first-column-comments \
            --format-all-comments \
            --comment-line-length78 \
            --line-comments-indentation0 \
            --comment-indentation33 \
            --declaration-comment-column33 \
            --no-comment-delimiters-on-blank-lines \
            --dont-star-comments \
            --blank-lines-after-declarations \
            --blank-lines-after-procedures \
            --else-endif-column33 \
            --braces-on-if-line \
            --cuddle-else \
            --case-indentation0 \
            --no-space-after-function-call-names \
            --procnames-start-lines \
            --space-after-cast \
            --blank-before-sizeof \
            --space-special-semicolon \
            --declaration-indentation1 \
            --no-blank-lines-after-commas \
            --indent-level2 \
            --continuation-indentation2 \
            --continue-at-parentheses \
            --parameter-indentation0


GINDENTFLAGS=--tab-size8 \
             --line-length78 \
             --leave-preprocessor-space \
             --format-first-column-comments \
             --format-all-comments \
             --comment-line-length78 \
             --line-comments-indentation0 \
             --comment-indentation33 \
             --declaration-comment-column33 \
             --no-comment-delimiters-on-blank-lines \
             --dont-break-procedure-type \
             --dont-star-comments \
             --blank-lines-after-declarations \
             --blank-lines-after-procedures \
             --else-endif-column33 \
             --braces-after-if-line \
             --dont-cuddle-else \
             --brace-indent0 \
             --case-indentation8 \
             --no-space-after-function-call-names \
             --space-after-cast \
             --blank-before-sizeof \
             --space-special-semicolon \
             --declaration-indentation1 \
             --no-blank-lines-after-commas \
             --indent-level8 \
             --continuation-indentation8 \
             --continue-at-parentheses \
             --parameter-indentation0

SOURCES = config.c amiga/console.c ncurses/console.c \
          unix-ansi/console.c const.c elan.c files.c global.h interf.h \
          symbols.c symtable.c texts.c config.h amiga/console.h \
          ncurses/console.h unix-ansi/console.h coredefs.h elan.h \
          files.h interf.c params.h symbols.h amiga/system.h \
          ncurses/system.h unix-ansi/system.h texts.h

all:
	@echo "Use 'make target' where target is:"
	@echo "    unix-ansi, ncurses or amiga"

ncurses::
	rm -f console.h console.c system.h
	ln -s ncurses/console.c; ln -s ncurses/console.h; ln -s ncurses/system.h

amiga::
	delete console.h cosole.c system.h
	makelink amiga/console.h
	makelink amiga/console.c
	makelink amiga/system.h

unix-ansi::
	rm -f console.h console.c system.h
	ln -s unix-ansi/console.c; ln -s unix-ansi/console.h; ln -s unix-ansi/system.h

ncurses amiga unix-ansi:: elan

clean:
	rm -f elan core *.o *.map *~ *.sav console.h console.c system.h doc/*.dvi

indent-mickey:
	indent ${INDENTFLAGS} $(SOURCES)

indent-giotti:
	indent ${GINDENTFLAGS} $(SOURCES)

# Dependencies:
elan: config.o console.o const.o elan.o files.o interf.o symbols.o symtable.o texts.o

elan.o: elan.c global.h coredefs.h texts.h symbols.h elan.h interf.h config.h

console.o: console.c global.h console.h

const.o: const.c global.h coredefs.h params.h

elan.o: elan.c global.h coredefs.h params.h texts.h symbols.h elan.h interf.h files.h

files.o: files.c global.h coredefs.h texts.h symbols.h elan.h interf.h files.h

interf.o: interf.c global.h coredefs.h params.h texts.h symbols.h elan.h interf.h config.h console.h 

symbols.o: symbols.c global.h coredefs.h symbols.h

symtable.o: symtable.c global.h coredefs.h symbols.h
 
texts.o: texts.c global.h coredefs.h texts.h
