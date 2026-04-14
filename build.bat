@echo off
setlocal enabledelayedexpansion

set "PATH=%~dp0..\cc65\bin;%PATH%"
set "PATH=%~dp0skysa;%PATH%"
set CC65_HOME=..\cc65\
set "PROJECT=%~1"
set "MAPPER=%~2"
set "out=%~3"
set "PROJECT_DIR=projects\%PROJECT%\"

if "%PROJECT%"=="" (
    echo No project specified.
    exit /b 1
)

if "%MAPPER%"=="" (
    echo No mapper specified.
    exit /b 1
)

if "%~4"=="--verbose" (
    set "VERBOSE=1"
) else (
    set "VERBOSE=0"
)

if /I "%MAPPER%"=="NROM" set DEFINES=-DNROM
if /I "%MAPPER%"=="CNROM" set DEFINES=-DCNROM
if /I "%MAPPER%"=="MMC3" set DEFINES=-DMMC3
echo Building project: "%PROJECT%", with mapper: "%MAPPER%"

if %VERBOSE%==1 (
    call skysa "%PROJECT_DIR%." --asmc -v
) else (
    call skysa "%PROJECT_DIR%." --asmc
)
del manifest_objs.txt 2>nul

cc65 -Oirs "%PROJECT_DIR%src\main.c" -I"." -I"%PROJECT_DIR%include" --add-source

if not exist "%PROJECT_DIR%src\main.s" pause

ca65 "%PROJECT_DIR%src\main.s" -g

for /f "usebackq delims=" %%F in ("%PROJECT_DIR%.srengine\manifest.txt") do (
    set "FILE=%PROJECT_DIR%%%F"
    set "EXT=%%~xF"

    if /i "!EXT!"==".c" (
        cc65 -Oirs "!FILE!" -I"." -I"%PROJECT_DIR%include" --add-source

        set "REL=%%F"
        set "SFILE=%PROJECT_DIR%!REL:.c=.s!"
        ca65 "!SFILE!" -g -I"." -I"%PROJECT_DIR%include"
    )

    if /i "!EXT!"==".s" (
        ca65 "!FILE!" -g -I"." -I"%PROJECT_DIR%include"
    )
)

ca65 %DEFINES% engine\common\startup_common.s -I"." -I"%PROJECT_DIR%."

for /f "usebackq delims=" %%F in ("%PROJECT_DIR%skysa_assembled.txt") do (
    ca65 "%%F" -g -I"." -I"%PROJECT_DIR%include"
)

>> manifest_objs.txt echo("%PROJECT_DIR%src\main.o"
>> manifest_objs.txt echo("engine\common\startup_common.o"


for /f "usebackq delims=" %%F in ("%PROJECT_DIR%.srengine\manifest.txt") do (
    set "EXT=%%~xF"
    set "REL=%%F"

    if  /i "!EXT!"==".c" (
        set "OFILE=%PROJECT_DIR%!REL:.c=.o!"
        >> manifest_objs.txt echo(!OFILE!
    )

    if /i "!EXT!"==".s" (
        set "OFILE=%PROJECT_DIR%!REL:.s=.o!"
        >> manifest_objs.txt echo(!OFILE!
    )
)

if %VERBOSE%==1 (
    echo manifest_objs.txt:
)
set manifest_objs=
for /f "usebackq delims=" %%O in ("manifest_objs.txt") do (
    if %VERBOSE%==1 (
        echo %%~O
    )
    set manifest_objs=!manifest_objs! "%%~O"
)

set objlist=
for /f "usebackq delims=" %%O in ("%PROJECT_DIR%skysa_objlist.txt") do (
    set objlist=!objlist! "%%~O"
)

mkdir "%PROJECT_DIR%build" 2>nul

ld65 -C engine\cfg\%MAPPER%.cfg -o %out% ^
    !manifest_objs! ^
    !objlist! ^
    nes.lib -Ln "%PROJECT_DIR%build\%out%.lbl"

echo Build complete. Output: "%PROJECT_DIR%build\%out%"

del "%PROJECT_DIR%src\main.o" 2>nul
del "engine\common\startup_common.o" 2>nul

for /f "usebackq delims=" %%F in ("%PROJECT_DIR%.srengine\manifest.txt") do (
    set "EXT=%%~xF"
    set "REL=%%F"
    if  /i "!EXT!"==".c" (
        set "OFILE=%PROJECT_DIR%!REL:.c=.o!"
        set "SFILE=%PROJECT_DIR%!REL:.c=.s!"
        del "!SFILE!"
        del "!OFILE!"
    )

    if /i "!EXT!"==".s" (
        set "OFILE=%PROJECT_DIR%!REL:.s=.o!"
        del "!OFILE!"
    )
)

for /f "usebackq delims=" %%F in ("%PROJECT_DIR%skysa_objlist.txt") do (
    del "%%~F"
)

if %VERBOSE%==1 (
    echo Removing file: manifest_objs.txt
)

del manifest_objs.txt 2>nul

if %VERBOSE%==1 (
    call skysa "%PROJECT_DIR%." --clean -v
) else (
    call skysa "%PROJECT_DIR%." --clean
)

move /Y "%PROJECT_DIR%src\main.s" "%PROJECT_DIR%build\" 
move /Y "%out%" "%PROJECT_DIR%build\"

"%PROJECT_DIR%build\%out%"

endlocal