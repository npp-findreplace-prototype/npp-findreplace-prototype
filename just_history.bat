@echo off
setlocal EnableExtensions
cd /d "%~dp0"

git log --oneline --decorate --graph -30

echo.
pause
exit /b 0