cd ..
copy os2\console.c . >nul
copy os2\console.h . >nul
copy os2\system.h . >nul
copy os2\elan.rc . >nul
copy os2\elan.def . >nul
copy os2\elan.ico . >nul
icc elan.c console.c config.c const.c files.c interf.c symbols.c symtable.c texts.c elan.def
rc elan
del console.c >nul
del console.h >nul
del system.h >nul
del elan.rc >nul
del elan.res >nul
del elan.def >nul
del elan.ico >nul

