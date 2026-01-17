@echo off
REM build.bat - Build script for TUI library examples (Windows)
REM
REM This script compiles all example programs using MSVC or MinGW.
REM Usage: build.bat

setlocal enabledelayedexpansion

echo ===================================
echo TUI Library Build Script (Windows)
echo ===================================
echo.

REM Create output directory
if not exist bin mkdir bin

REM Try to find MSVC
where cl >nul 2>nul
if %errorlevel% equ 0 (
    echo Using MSVC compiler
    echo.
    goto :msvc_build
)

REM Try to find MinGW
where gcc >nul 2>nul
if %errorlevel% equ 0 (
    echo Using MinGW GCC compiler
    echo.
    goto :mingw_build
)

echo Error: No C compiler found.
echo Please run from Visual Studio Developer Command Prompt
echo or install MinGW and add it to PATH.
exit /b 1

:msvc_build
echo Building with MSVC...
echo -----------------------------------

echo Building hello...
cl /nologo /std:c11 /W4 /O2 /Fe:bin\hello.exe examples\hello.c /link user32.lib
if %errorlevel% neq 0 goto :error

echo Building buttons...
cl /nologo /std:c11 /W4 /O2 /Fe:bin\buttons.exe examples\buttons.c /link user32.lib
if %errorlevel% neq 0 goto :error

echo Building input...
cl /nologo /std:c11 /W4 /O2 /Fe:bin\input.exe examples\input.c /link user32.lib
if %errorlevel% neq 0 goto :error

REM Clean up object files
del /q *.obj 2>nul

goto :done

:mingw_build
echo Building with MinGW...
echo -----------------------------------

echo Building hello...
gcc -std=c11 -Wall -Wextra -O2 -o bin\hello.exe examples\hello.c
if %errorlevel% neq 0 goto :error

echo Building buttons...
gcc -std=c11 -Wall -Wextra -O2 -o bin\buttons.exe examples\buttons.c
if %errorlevel% neq 0 goto :error

echo Building input...
gcc -std=c11 -Wall -Wextra -O2 -o bin\input.exe examples\input.c
if %errorlevel% neq 0 goto :error

goto :done

:error
echo.
echo Build failed!
exit /b 1

:done
echo -----------------------------------
echo.
echo Build complete! Binaries are in bin\
echo.
echo Run examples:
echo   bin\hello.exe    - Label rendering demo
echo   bin\buttons.exe  - Interactive buttons demo
echo   bin\input.exe    - Keyboard handling demo
echo.

endlocal
