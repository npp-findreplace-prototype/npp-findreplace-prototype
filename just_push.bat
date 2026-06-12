@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo.
echo === Push local commits to GitHub ===
echo.

if not exist ".git" (
    echo ERROR: This folder is not a Git repository.
    echo %CD%
    echo.
    pause
    exit /b 1
)

git status -sb
echo.

git push
if errorlevel 1 (
    echo.
    echo ERROR: Push failed.
    echo Run git-status-check.bat to inspect the state.
    echo.
    pause
    exit /b 1
)

echo.
echo Push complete.
echo.
pause
exit /b 0