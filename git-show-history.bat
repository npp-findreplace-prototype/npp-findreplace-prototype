@echo off
setlocal EnableExtensions

cd /d "%~dp0"

echo.
echo === Recent Git history ===
echo.

if not exist ".git" (
    echo ERROR: This folder is not a Git repository.
    pause
    exit /b 1
)

git log --oneline --decorate --graph -30

echo.
pause
exit /b 0