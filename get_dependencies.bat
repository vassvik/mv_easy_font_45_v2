@echo off 

rem Create file directories, and clean them up if they  exist

if exist "dependencies" rmdir /S /Q dependencies
mkdir dependencies

if not exist "include" mkdir include

if exist "include\GLFW" rmdir /S /Q include\GLFW
mkdir include\GLFW

if exist "include\stb" rmdir /S /Q include\stb
mkdir include\stb

if not exist "lib" mkdir lib
if exist "lib\glfw3.lib" del lib\glfw3.lib

rem Cet all dependencies, build and copy over files
pushd dependencies
    REM Clone repos
    git clone https://github.com/glfw/glfw
    git clone https://github.com/nothings/stb.git

    REM Build glfw
    pushd glfw
        mkdir build 
        pushd build 
            cmake -G "NMake Makefiles" .. -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DUSE_MSVC_RUNTIME_LIBRARY_DLL=OFF
        popd
        cmake --build build --config debug

        REM copy files
        copy build\src\glfw3.lib ..\..\lib\glfw3.lib
        copy include\GLFW\*.h ..\..\include\GLFW\
    popd

    REM Copy stb files
    copy stb\stb_truetype.h ..\include\stb\
    copy stb\stb_image_write.h ..\include\stb\
    copy stb\stb_rect_pack.h ..\include\stb\

popd