@echo off
setlocal EnableExtensions
cd /d "%~dp0"

if exist "git-get-latest.bat" (
    call git-get-latest.bat
) else (
    git fetch
    git merge --ff-only @{u}
    pause
)

endlocal
exit /b %ERRORLEVEL%