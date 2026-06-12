@echo off
setlocal EnableExtensions
cd /d "%~dp0"

set "login.rc=0"
set "login.config_file=build_config.bat"
set "login.default_name=npp-findreplace-prototype"
set "login.default_email=npp-findreplacedialog-prototype@jfperreault.ca"
set "login.repo_url=https://github.com/npp-findreplace-prototype/npp-findreplace-prototype.git"
set "login.branch=main"

if exist "%login.config_file%" call "%login.config_file%"
if defined app.repo_url set "login.repo_url=%app.repo_url%"

echo.
echo ============================================================
echo  GitHub first-login / first-push setup
echo ============================================================
echo.

where git >nul 2>nul
if errorlevel 1 (
    echo ERROR: git was not found in PATH.
    echo Install Git for Windows first.
    echo.
    pause
    exit /b 1
)

if not exist ".git" (
    echo ERROR: This folder is not a Git repository:
    echo %CD%
    echo.
    pause
    exit /b 1
)

echo This script will:
echo   set local Git name/email for this repo
echo   set the origin GitHub URL
echo   configure Git Credential Manager if available
echo   push the current branch with upstream tracking
echo.
echo GitHub password is not stored by this script.
echo Git will open a browser login or ask for a token during push.
echo.
pause

echo.
set "login.name="
set /p "login.name=Git name [%login.default_name%]: "
if "%login.name%"=="" set "login.name=%login.default_name%"

set "login.email="
set /p "login.email=Git email [%login.default_email%]: "
if "%login.email%"=="" set "login.email=%login.default_email%"

echo.
echo Repo URL:
echo   %login.repo_url%
echo.
set "login.change_repo="
set /p "login.change_repo=Use this repo URL? [Y/n]: "
if /I "%login.change_repo%"=="n" (
    set "login.repo_url="
    set /p "login.repo_url=Enter GitHub repo URL: "
)

if "%login.repo_url%"=="" (
    echo.
    echo ERROR: Repo URL is required.
    echo.
    pause
    exit /b 1
)

echo.
echo Setting local Git identity...
git config --local user.name "%login.name%"
if errorlevel 1 ( echo ERROR: Could not set user.name. & pause & exit /b 1 )

git config --local user.email "%login.email%"
if errorlevel 1 ( echo ERROR: Could not set user.email. & pause & exit /b 1 )

echo.
echo Checking Git Credential Manager...
git credential-manager --version >nul 2>nul
if not errorlevel 1 (
    git config --global credential.helper manager
    echo Git Credential Manager configured.
) else (
    git credential-manager-core --version >nul 2>nul
    if not errorlevel 1 (
        git config --global credential.helper manager-core
        echo Git Credential Manager Core configured.
    ) else (
        echo WARNING: Git Credential Manager was not found.
        echo Git may ask for a username and token in the console.
    )
)

echo.
echo Setting origin remote...
git remote get-url origin >nul 2>nul
if errorlevel 1 (
    git remote add origin "%login.repo_url%"
) else (
    git remote set-url origin "%login.repo_url%"
)

if errorlevel 1 (
    echo.
    echo ERROR: Could not set origin remote.
    echo.
    pause
    exit /b 1
)

for /f "delims=" %%B in ('git branch --show-current') do set "login.current_branch=%%B"

if "%login.current_branch%"=="" (
    echo.
    echo No current branch detected. Creating main branch...
    git checkout -B "%login.branch%"
    if errorlevel 1 (
        echo.
        echo ERROR: Could not create or switch to %login.branch%.
        echo.
        pause
        exit /b 1
    )
    set "login.current_branch=%login.branch%"
)

echo.
echo Current Git setup:
echo ------------------------------------------------------------
git config --local --get user.name
git config --local --get user.email
git remote -v
git status -sb
echo ------------------------------------------------------------
echo.

echo Ready for first push.
echo During push, GitHub may open a browser login window.
echo.
pause

git push -u origin "%login.current_branch%"
if errorlevel 1 (
    echo.
    echo ERROR: First push/login failed.
    echo.
    echo If Git asks for a password, use a GitHub personal access token,
    echo not your GitHub account password.
    echo.
    pause
    exit /b 1
)

echo.
echo GitHub login and first push are set up.
echo Future pushes should work with:
echo   just_push.bat
echo   build.bat sync
echo.
pause
exit /b 0