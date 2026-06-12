@echo off
setlocal EnableExtensions
cd /d "%~dp0"

call build.bat check

endlocal
exit /b %ERRORLEVEL%