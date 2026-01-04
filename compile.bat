@echo off
set out="bitlanders"
set path=%path%;..\..\cc65\bin\
set CC65_HOME=..\..\cc65\

cc65 -Oirs src\main.c -Iinclude --add-source
ca65 linker\crt0.s

if not exist src\main.s pause

ca65 src\main.s -g
ld65 -C linker\rom.cfg -o %out%.nes linker\crt0.o src\main.o nes.lib -Ln build\\%out%.lbl

del src\main.o
del linker\crt0.o

move /Y src\main.s build\ 
move /Y %out%.nes build\

pause
build\\%out%.nes