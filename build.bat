@echo off
setlocal enabledelayedexpansion

set out="bitlanders"
set path=%path%;..\cc65\bin\
set CC65_HOME=..\cc65\
set path=%path%;compiler\

call skysa --asmc

cc65 -Oirs src\main.c -Iinclude --add-source
cc65 -Oirs src\characters\23_wreckingball.c -Iinclude --add-source
cc65 -Oirs src\hats\11_tophat.c -Iinclude --add-source
cc65 -Oirs src\hats\17_bone.c -Iinclude --add-source
cc65 -Oirs src\hats\22_cowboy.c -Iinclude --add-source
cc65 -Oirs src\water\water.c -Iinclude --add-source

ca65 linker\crt0.s

if not exist src\main.s pause

ca65 src\main.s -g
ca65 src\characters\23_wreckingball.s -g
ca65 src\hats\11_tophat.s -g
ca65 src\hats\17_bone.s -g
ca65 src\hats\22_cowboy.s -g
ca65 src\water\water.s -g
ca65 src\water\water_irq.s -g

for /f %%F in (skysa_assembled.txt) do (
    ca65 %%F -g
)

set /p objlist=<skysa_objlist.txt

ld65 -C linker\rom.cfg -o %out%.nes ^
    linker\crt0.o ^
    src\main.o ^
    src\characters\23_wreckingball.o ^
    src\hats\11_tophat.o ^
    src\hats\17_bone.o ^
    src\hats\22_cowboy.o ^
    src\water\water.o ^
    src\water\water_irq.o ^
    !objlist! ^
    nes.lib -Ln build\\%out%.lbl

del src\main.o
del src\characters\23_wreckingball.o
del src\hats\11_tophat.o
del src\hats\17_bone.o
del src\hats\22_cowboy.o
del src\water\water.o
del src\water\water_irq.o
del linker\crt0.o

del src\characters\23_wreckingball.s
del src\hats\11_tophat.s
del src\hats\17_bone.s
del src\hats\22_cowboy.s
del src\water\water.s

for /f %%F in (skysa_objlist.txt) do (
    del %%F
)

call skysa --clean
move /Y src\main.s build\ 
move /Y %out%.nes build\

build\\%out%.nes

endlocal