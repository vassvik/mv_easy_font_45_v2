@echo off 

REM @TODO(mv): figure out how to properly do CRT files in debug and release mode without warnings

cd %~dp0

utils\otime -begin timings.otm

cl /nologo /MP^
 /Iinclude^
 src\main.c^
 gdi32.lib ^
 shell32.lib^
 lib\glfw3.lib^
 user32.lib^
 msvcrtd.lib^
 libcmt.lib

del *.obj

utils\otime -end timings.otm