@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo.
echo === Commit local changes only ===
echo.

if not exist ".git" (
    echo ERROR: This folder is not a Git repository.
    echo %CD%
    echo.
    pause
    exit /b 1
)

set DIRTY=
for /f "delims=" %%A in ('git status --porcelain') do set DIRTY=1

if not defined DIRTY (
    echo No local changes to commit.
    echo.
    pause
    exit /b 0
)

echo Current changes:
git status --short
echo.

:AskMessage
set "MSG="
set /p "MSG=Commit message: "

if "%MSG%"=="" (
    echo Commit message is required.
    echo.
    goto AskMessage
)

git add --all
if errorlevel 1 (
    echo.
    echo ERROR: git add failed.
    echo.
    pause
    exit /b 1
)

git commit -m "%MSG%"
if errorlevel 1 (
    echo.
    echo ERROR: git commit failed.
    echo.
    pause
    exit /b 1
)

echo.
echo Commit complete. Not pushed yet.
echo To push later, run:
echo   just_push.bat
echo.
pause
exit /b 0