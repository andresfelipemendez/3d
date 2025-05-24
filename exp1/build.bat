@echo off
setlocal enabledelayedexpansion

:: Simple build script - compile main.c with SDL3 precompiled binaries
:: Run from Visual Studio Developer Command Prompt

echo Compiling with SDL3...

:: Check MSVC environment
if "%VCINSTALLDIR%"=="" (
    echo Error: Run from Visual Studio Developer Command Prompt
    pause
    exit /b 1
)

:: SDL3 paths (adjust these to match your SDL3 installation)
set SDL_INCLUDE=lib\SDL3\include
set SDL_LIB=lib\SDL3\lib\x64

:: Compiler flags
set CFLAGS=/nologo /O2 /W3 /MD /std:c11
set INCLUDES=/I"%SDL_INCLUDE%"
set LIBS="%SDL_LIB%\SDL3.lib"

:: System libraries that SDL3 needs
set SYSLIBS=user32.lib gdi32.lib winmm.lib imm32.lib ole32.lib oleaut32.lib shell32.lib setupapi.lib version.lib uuid.lib

echo Compiling main.c...
cl %CFLAGS% %INCLUDES% /Febuild\game.exe main.c glad.c /link %LIBS% %SYSLIBS% /SUBSYSTEM:CONSOLE 

if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

if not exist "build\SDL3.dll" (
    if exist "%SDL_LIB%\SDL3.dll" (
        echo Copying SDL3.dll to build directory...
        copy "%SDL_LIB%\SDL3.dll" "build\"
    ) else (
        echo Warning: SDL3.dll not found in %SDL_LIB%
        echo You'll need to copy it manually to the build directory
    )
) else (
    echo SDL3.dll already exists in build directory
)
