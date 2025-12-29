@echo off
set out="bitlanders"
set path=%path%;..\..\cc65\bin\
set CC65_HOME=..\..\cc65\

cc65 -Oirs src\main.c -Iinclude --add-source
ca65 linker\crt0.s
ca65 lib\libskyretro\skyretro.s -o lib\libskyretro\skyretro.o
if not exist src\main.s pause

ca65 src\main.s -g
ld65 -C linker\rom.cfg -o %out%.nes linker\crt0.o src\main.o lib\libskyretro\skyretro.o nes.lib -Ln build\\%out%.lbl

del src\main.o
del linker\crt0.o
del lib\libskyretro\skyretro.o

move /Y src\main.s build\ 
move /Y %out%.nes build\

pause
build\\%out%.nes