@echo off
setlocal EnableExtensions EnableDelayedExpansion

set APP_NAME=window
set EXE_NAME=window.exe

set TIME_FORMAT=yyyy-MM-dd.HH'h'mm's'ss
set LIBS=-luser32 -lgdi32 -lcomctl32 -lgdiplus -lole32
set TCC_FLAGS=-mwindows
set THEME_ROOT=themes
set THEME_GENERATED_C=theme_resources_generated.c

for /f %%A in ('powershell -NoProfile -Command "Get-Date -Format \"%TIME_FORMAT%\""') do set BUILD_STAMP=%%A

set BUILD_DIR=build_%BUILD_STAMP%
set SOURCE_DIR=source_%BUILD_STAMP%
set OLDBUILDS_DIR=oldbuilds
set OUTPUT_EXE=%BUILD_DIR%\%EXE_NAME%
set CURRENT_HARDLINK=%EXE_NAME%

if not exist "%OLDBUILDS_DIR%" mkdir "%OLDBUILDS_DIR%"

for /d %%D in (build_*) do if /i not "%%~nxD"=="%BUILD_DIR%" move "%%D" "%OLDBUILDS_DIR%\%%~nxD" >nul
for /d %%D in (source_*) do if /i not "%%~nxD"=="%SOURCE_DIR%" move "%%D" "%OLDBUILDS_DIR%\%%~nxD" >nul

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%SOURCE_DIR%" mkdir "%SOURCE_DIR%"

echo Generating embedded theme resource source...
powershell -NoProfile -ExecutionPolicy Bypass -File generate_theme_resources.ps1 -ThemeRoot "%THEME_ROOT%" -OutFile "%THEME_GENERATED_C%"
if errorlevel 1 echo Theme resource generation failed. & pause & exit /b 1

set CFILES=
for %%F in (*.c) do set CFILES=!CFILES! "%%F"

echo Building to %OUTPUT_EXE%...
tcc %TCC_FLAGS% -o "%OUTPUT_EXE%" !CFILES! %LIBS%
if errorlevel 1 echo Build failed. & pause & exit /b 1

echo Copying source snapshot to %SOURCE_DIR%...
robocopy . "%SOURCE_DIR%" /E /XD build_* source_* oldbuilds /XF "%EXE_NAME%" >nul
if %ERRORLEVEL% GEQ 8 echo Source copy failed. & pause & exit /b 1

echo Copying source snapshot into %BUILD_DIR%\%SOURCE_DIR%...
robocopy "%SOURCE_DIR%" "%BUILD_DIR%\%SOURCE_DIR%" /E >nul
if %ERRORLEVEL% GEQ 8 echo Build source copy failed. & pause & exit /b 1

if exist "%CURRENT_HARDLINK%" del "%CURRENT_HARDLINK%"
mklink /H "%CURRENT_HARDLINK%" "%OUTPUT_EXE%" >nul
if errorlevel 1 copy /Y "%OUTPUT_EXE%" "%CURRENT_HARDLINK%" >nul

echo Build complete.
echo Output: %OUTPUT_EXE%
echo Link:   %CURRENT_HARDLINK%

endlocal
pause