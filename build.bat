@echo off

set path=E:\dev\ed;%path%

set SOURCES=main.cpp

start /wait /B cl /MP /Fdbuild\ /Febuild/ed /Fobuild\  /EHsc /Zi /FC /wd"4838" /nologo /W1 /Od /Iinclude %SOURCES% /link opengl32.lib winmm.lib gdi32.lib lib\SDL2.lib lib\SDL2main.lib lib\SDL2_ttf.lib /SUBSYSTEM:CONSOLE

start /wait build/ed.exe