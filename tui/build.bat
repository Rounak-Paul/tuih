@echo off
REM build.bat - Build script for TUI library (Windows)
REM
REM This script compiles the demo program using MSVC or MinGW.
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

echo Building demo...
cl /nologo /std:c11 /W4 /O2 /Fe:bin\demo.exe examples\demo.c /link user32.lib
if %errorlevel% neq 0 goto :error

REM Clean up object files
del /q *.obj 2>nul

goto :done

:mingw_build
echo Building with MinGW...
echo -----------------------------------

echo Building demo...
gcc -std=c11 -Wall -Wextra -O2 -o bin\demo.exe examples\demo.c
if %errorlevel% neq 0 goto :error

goto :done

:error
echo.
echo Build failed!
exit /b 1

:done
echo -----------------------------------
echo.
echo Build complete! Binary is in bin\
echo.
echo Run demo:
echo   bin\demo.exe  - Complete TUI library demo
echo.

endlocal
