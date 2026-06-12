@echo off
setlocal EnableExtensions
cd /d "%~dp0"

if exist "git-status-check.bat" (
    call git-status-check.bat
) else (
    git status -sb
    pause
)

endlocal
exit /b %ERRORLEVEL%