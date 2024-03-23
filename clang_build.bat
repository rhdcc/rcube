@ECHO OFF
REM Build Project (with clang)
SETLOCAL ENABLEDELAYEDEXPANSION

SET assembly=rcube.exe
SET source_dir=src
SET include_dir=include
SET raylib_dir=lib/raylib
SET lib_dir=lib/raylib_build
SET bin_dir=bin

SET sources=src/main.c src/rubik.c

REM Libraries to link with
SET libs=-L%lib_dir% -lraylib -lUser32 -lopengl32 -lwinmm -lgdi32 -lkernel32 -lshell32

REM External header files
SET deps=-I%include_dir%/ -I%raylib_dir%/src/ -I%raylib_dir%/src/external/glfw/include/

IF NOT EXIST %bin_dir% MKDIR %bin_dir%
clang -o %bin_dir%/%assembly% -DPLATFORM_DESKTOP -D_CRT_SECURE_NO_WARNINGS %deps% !sources! %libs%