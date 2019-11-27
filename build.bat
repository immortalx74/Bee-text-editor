@echo off

set path=E:\dev\ed;%path%

set SOURCES=main.cpp character.cpp draw.cpp line.cpp globals.cpp input.cpp
set LINK=opengl32.lib winmm.lib gdi32.lib lib\SDL2.lib lib\SDL2main.lib lib\SDL2_ttf.lib
set CFLAGS=/MP /EHsc /Zi /FC /nologo /W1 /Od
set SKIP_WARNINGS=/wd"4838"
set BUILD_DIR=/Fdbuild\ /Fobuild\
set EXE=build/ed.exe

start /wait /B cl %CFLAGS% %SKIP_WARNINGS% %BUILD_DIR% /Fe%EXE% /Iinclude %SOURCES% /link %LINK% /SUBSYSTEM:CONSOLE

start /wait %EXE%