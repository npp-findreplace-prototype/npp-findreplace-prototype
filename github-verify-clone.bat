@echo off
setlocal EnableExtensions EnableDelayedExpansion

cd /d "%~dp0"

echo.
echo === Verify GitHub clone ===
echo.

if not exist ".git" (
    echo ERROR: This folder is not a Git repository.
    pause
    exit /b 1
)

for /f "delims=" %%A in ('git remote get-url origin') do set REMOTE_URL=%%A

if "%REMOTE_URL%"=="" (
    echo ERROR: Could not determine origin URL.
    pause
    exit /b 1
)

set TEST_DIR=%TEMP%\findreplacemark-verify-clone-%RANDOM%-%RANDOM%

echo Cloning into:
echo %TEST_DIR%
echo.

git clone "%REMOTE_URL%" "%TEST_DIR%"
if errorlevel 1 (
    echo.
    echo ERROR: Test clone failed.
    echo.
    pause
    exit /b 1
)

echo.
echo Test clone succeeded.
echo.

dir "%TEST_DIR%"

echo.
echo Removing test clone...
rmdir /S /Q "%TEST_DIR%"

echo.
echo GitHub verification complete.
echo.
pause
exit /b 0