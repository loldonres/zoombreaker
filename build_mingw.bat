@echo off
setlocal
cd /d "%~dp0"

if not defined MINGW_BIN set "MINGW_BIN=C:\Qt\Tools\mingw1310_64\bin"
if not exist "%MINGW_BIN%\g++.exe" (
    echo MinGW not found: "%MINGW_BIN%"
    echo Set MINGW_BIN to the MinGW bin folder and run this file again.
    pause
    exit /b 1
)

if not exist ".build-mingw" mkdir ".build-mingw"

echo [1/3] Embedding application resources and icon...
"%MINGW_BIN%\windres.exe" -i app_resources.rc -o .build-mingw\app_resources.o --output-format=coff
if errorlevel 1 goto failed

echo [2/3] Compiling C source files...
"%MINGW_BIN%\gcc.exe" -std=c11 -O2 -c stb_vorbis_impl.c -o .build-mingw\stb_vorbis_impl.o
if errorlevel 1 goto failed
"%MINGW_BIN%\gcc.exe" -std=c11 -O2 -c uia_guids.c -o .build-mingw\uia_guids.o
if errorlevel 1 goto failed

echo [3/3] Compiling and linking ZoomBreaker...
"%MINGW_BIN%\g++.exe" -std=c++17 -O2 -static -municode -mwindows ^
    main.cpp settings.cpp bass_boost.cpp http_client.cpp neon_api.cpp zoom_parse.cpp utf8_utils.cpp ^
    .build-mingw\stb_vorbis_impl.o .build-mingw\uia_guids.o .build-mingw\app_resources.o ^
    -o ZoomBreaker-zoombreaker-icon.exe ^
    -lcomctl32 -lcomdlg32 -ldwmapi -lole32 -loleaut32 -lshlwapi -lshell32 ^
    -luxtheme -lwinhttp -lwinmm -lksuser -ldsound -lsetupapi
if errorlevel 1 goto failed

echo.
echo Build complete: ZoomBreaker-zoombreaker-icon.exe
pause
exit /b 0

:failed
echo.
echo Build failed. Check the error above.
pause
exit /b 1
