@echo off
setlocal enabledelayedexpansion

set out="bitlanders"
set path=%path%;..\..\cc65\bin\
set CC65_HOME=..\..\cc65\
set path=%path%;tools\

call skyc --asmc

cc65 -Oirs src\main.c -Iinclude --add-source
ca65 linker\crt0.s

if not exist src\main.s pause

ca65 src\main.s -g

for /f %%F in (skyc_assembled.txt) do (
    ca65 %%F -g
)

set /p objlist=<skyc_objlist.txt

ld65 -C linker\rom.cfg -o %out%.nes ^
    linker\crt0.o ^
    src\main.o ^
    !objlist! ^
    nes.lib -Ln build\\%out%.lbl

del src\main.o
del linker\crt0.o

for /f %%F in (skyc_objlist.txt) do (
    del %%F
)

call skyc --clean

move /Y src\main.s build\ 
move /Y %out%.nes build\

pause
build\\%out%.nes

endlocal