@echo off
setlocal enabledelayedexpansion

set "EXE=window.exe"
set "OLD_DIR=oldbuild"

rem Example timestamp: 2026-06-04.15h35s07
set "TIME_FORMAT=yyyy-MM-dd.HH'h'mm's'ss"

set "TCC_MODE=-mwindows"
set "TCC_LIBS=-luser32 -lgdi32"

for /f %%I in ('powershell -NoProfile -Command "Get-Date -Format $env:TIME_FORMAT"') do set "STAMP=%%I"

set "BUILD_DIR=build_%STAMP%"
set "SOURCE_DIR=source_%STAMP%"

if not exist "%OLD_DIR%" mkdir "%OLD_DIR%"

rem Move previous build_* and source_* folders into oldbuild\
for /d %%D in (build_* source_*) do if exist "%%D" if exist "%OLD_DIR%\%%~nxD" (move "%%D" "%OLD_DIR%\%%~nxD_archived_%STAMP%" >nul) else (move "%%D" "%OLD_DIR%\" >nul)

mkdir "%BUILD_DIR%"
mkdir "%SOURCE_DIR%"

rem Automatically compile every .c file in the current folder.
set "SOURCES="
for %%F in (*.c) do set SOURCES=!SOURCES! "%%F"

if not defined SOURCES echo No .c source files found.& pause& exit /b 1

echo Building to %BUILD_DIR%\%EXE%...
echo Sources:%SOURCES%

tcc %SOURCES% %TCC_MODE% %TCC_LIBS% -o "%BUILD_DIR%\%EXE%"

if errorlevel 1 echo Build failed.& pause& exit /b 1

rem Copy project files into source_ timestamp folder.
rem Includes this .bat file.
rem Excludes build_* folders, source_* folders, and oldbuild.
robocopy "." "%SOURCE_DIR%" /E /XD "build_*" "source_*" "%OLD_DIR%" /NFL /NDL /NJH /NJS /NC /NS

if errorlevel 8 echo Source copy failed.& pause& exit /b 1

rem Update hardlink in current folder.
if exist "%EXE%" del /f /q "%EXE%"

mklink /H "%EXE%" "%BUILD_DIR%\%EXE%"

if errorlevel 1 echo Build succeeded, but hardlink creation failed.& echo Built exe is here: %BUILD_DIR%\%EXE%& pause& exit /b 1

echo Build successful.
echo Output: %BUILD_DIR%\%EXE%
echo Source snapshot: %SOURCE_DIR%
echo Hardlink: %EXE%
pause