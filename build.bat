@echo off

set path=E:\dev\ed;%path%

set SOURCES=main.cpp character.cpp draw.cpp line.cpp globals.cpp input.cpp file.cpp helpers.cpp list.cpp clipboard.cpp undoredo.cpp
set LINK=opengl32.lib winmm.lib gdi32.lib lib\SDL2.lib lib\SDL2main.lib lib\SDL2_ttf.lib

REM DEBUG
set CFLAGS=/MP /EHsc /Zi /FC /nologo /W1 /Od

REM RELEASE
REM set CFLAGS=/MP /EHsc /FC /nologo /W1 /O2

REM set SKIP_WARNINGS=/wd"4838"
set BUILD_DIR=/Fdbuild\ /Fobuild\
set EXE=build/linearbee.exe

start /wait /B cl %CFLAGS% %BUILD_DIR% /Fe%EXE% /Iinclude %SOURCES% /link %LINK% /SUBSYSTEM:CONSOLE

IF %ERRORLEVEL% EQU 0 start /wait %EXE%