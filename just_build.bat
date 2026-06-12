@echo off
setlocal EnableExtensions
cd /d "%~dp0"

call build.bat nosync

endlocal
exit /b %ERRORLEVEL%