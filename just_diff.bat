@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo.
echo === Changed files ===
echo.

git status --short

echo.
echo === Diff summary ===
echo.

git diff --stat

echo.
pause
exit /b 0