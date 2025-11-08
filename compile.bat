@echo off
set name="game"
set out="bitlanders"
set path=%path%;..\..\cc65\bin\
set CC65_HOME=..\..\cc65\

cc65 -Oirs %name%.c --add-source
ca65 crt0.s
if not exist %name%.s pause
ca65 %name%.s -g
ld65 -C cnrom_32k_vert.cfg -o %out%.nes crt0.o %name%.o nes.lib -Ln labels.txt

del *.o
move /Y labels.txt build\ 
move /Y %name%.s build\ 
move /Y %out%.nes build\
pause
build\%out%.nes