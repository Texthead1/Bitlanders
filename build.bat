@echo off
setlocal enabledelayedexpansion

set out="bitlanders"
set path=%path%;..\..\cc65\bin\
set CC65_HOME=..\..\cc65\
set path=%path%;compiler\

call skyas --asmc

cc65 -Oirs src\main.c -Iinclude --add-source
cc65 -Oirs src\characters\bean.c -Iinclude --add-source
cc65 -Oirs src\hats\11_tophat.c -Iinclude --add-source
cc65 -Oirs src\hats\17_bone.c -Iinclude --add-source
cc65 -Oirs src\hats\22_cowboy.c -Iinclude --add-source

ca65 linker\crt0.s

if not exist src\main.s pause

ca65 src\main.s -g
ca65 src\characters\bean.s -g
ca65 src\hats\11_tophat.s -g
ca65 src\hats\17_bone.s -g
ca65 src\hats\22_cowboy.s -g

for /f %%F in (skyas_assembled.txt) do (
    ca65 %%F -g
)

set /p objlist=<skyas_objlist.txt

ld65 -C linker\rom.cfg -o %out%.nes ^
    linker\crt0.o ^
    src\main.o ^
    src\characters\bean.o ^
    src\hats\11_tophat.o ^
    src\hats\17_bone.o ^
    src\hats\22_cowboy.o ^
    !objlist! ^
    nes.lib -Ln build\\%out%.lbl

del src\main.o
del src\characters\bean.o
del src\hats\11_tophat.o
del src\hats\17_bone.o
del src\hats\22_cowboy.o
del linker\crt0.o

del src\characters\bean.s
del src\hats\11_tophat.s
del src\hats\17_bone.s
del src\hats\22_cowboy.s

for /f %%F in (skyas_objlist.txt) do (
    del %%F
)

call skyas --clean

move /Y src\main.s build\ 
move /Y %out%.nes build\

build\\%out%.nes

endlocal