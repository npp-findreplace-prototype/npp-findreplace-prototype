@echo off
setlocal enabledelayedexpansion

set "SRC=window.c"
set "EXE=window.exe"

set "OLD_DIR=oldbuild"

for /f %%I in ('powershell -NoProfile -Command "Get-Date -Format yyyy-MM-dd.HH'h'mm's'ss"') do set "STAMP=%%I"

set "BUILD_DIR=build_%STAMP%"
set "SOURCE_DIR=source_%STAMP%"

if not exist "%OLD_DIR%" mkdir "%OLD_DIR%"

rem Move previous build_* and source_* folders into oldbuild\
for /d %%D in (build_* source_*) do (
    if exist "%%D" (
        if exist "%OLD_DIR%\%%~nxD" (
            move "%%D" "%OLD_DIR%\%%~nxD_archived_%STAMP%" >nul
        ) else (
            move "%%D" "%OLD_DIR%\" >nul
        )
    )
)

mkdir "%BUILD_DIR%"
mkdir "%SOURCE_DIR%"

echo Building to %BUILD_DIR%\%EXE%...

tcc "%SRC%" -mwindows -luser32 -lgdi32 -o "%BUILD_DIR%\%EXE%"

if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

rem Copy project files into source_ timestamp folder.
rem Excludes build_* and source_* folders, and oldbuild.
robocopy "." "%SOURCE_DIR%" /E /XD "build_*" "source_*" "%OLD_DIR%" /NFL /NDL /NJH /NJS /NC /NS

if errorlevel 8 (
    echo Source copy failed.
    pause
    exit /b 1
)

rem Update hardlink in current folder
if exist "%EXE%" del /f /q "%EXE%"

mklink /H "%EXE%" "%BUILD_DIR%\%EXE%"

if errorlevel 1 (
    echo Build succeeded, but hardlink creation failed.
    echo Built exe is here: %BUILD_DIR%\%EXE%
    pause
    exit /b 1
)

echo Build successful.
echo Output: %BUILD_DIR%\%EXE%
echo Source snapshot: %SOURCE_DIR%
echo Hardlink: %EXE%
pause