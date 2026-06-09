@echo off
setlocal EnableExtensions

cd /d "%~dp0"

echo.
echo === Push local commits to GitHub ===
echo.

if not exist ".git" (
    echo ERROR: This folder is not a Git repository.
    pause
    exit /b 1
)

git status -sb
echo.

git push
if errorlevel 1 (
    echo.
    echo ERROR: Push failed.
    echo Possible causes:
    echo - internet problem
    echo - GitHub login needed
    echo - GitHub has newer commits
    echo.
    echo Run:
    echo   git-status-check.bat
    echo.
    pause
    exit /b 1
)

echo.
echo Push complete.
echo.
pause
exit /b 0