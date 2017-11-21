@echo off 

REM @TODO(mv): figure out how to properly do CRT files in debug and release mode without warnings

cd %~dp0

utils\otime -begin timings.otm

cl /nologo /MTd /MP^
 /Iinclude^
 src\main.c^
 lib\glfw3.lib^
 shell32.lib^
 user32.lib^
 gdi32.lib

del *.obj

utils\otime -end timings.otm