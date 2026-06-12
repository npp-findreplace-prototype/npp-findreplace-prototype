@echo off
setlocal EnableExtensions
cd /d "%~dp0"

if exist "github-verify-clone.bat" (
    call github-verify-clone.bat
) else (
    echo ERROR: github-verify-clone.bat was not found.
    echo.
    pause
    exit /b 1
)

endlocal
exit /b %ERRORLEVEL%