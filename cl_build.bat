@ECHO OFF
REM Build Project (with cl)
SETLOCAL ENABLEDELAYEDEXPANSION

SET assembly=rcube.exe
SET source_dir=src
SET include_dir=include
SET raylib_dir=lib\raylib
SET lib_dir=lib\raylib_build
SET bin_dir=bin

SET sources=src\main.c

REM Libraries to link with
SET libs=/link /LIBPATH:%lib_dir% raylib.lib User32.lib opengl32.lib winmm.lib gdi32.lib kernel32.lib shell32.lib 

REM External header files
SET deps=/I%include_dir%\ /I%raylib_dir%\src\ /I%raylib_dir%\src\external\glfw\include\

IF NOT EXIST %bin_dir% MKDIR %bin_dir%
cl /nologo /Fe%bin_dir%/%assembly% /Fo%bin_dir%\ /Fd%bin_dir%\ /DPLATFORM_DESKTOP /D_CRT_SECURE_NO_WARNINGS %deps% !sources! %libs%