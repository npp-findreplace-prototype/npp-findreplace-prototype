@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"

call :SetBFTColours >nul 2>nul
if errorlevel 1 call :SetNoColours

set "COMMIT_AFTER_BUILD=1"
set "PUSH_AFTER_BUILD=1"
set "CHECK_ONLY=0"
set "HELP_ONLY=0"
set "BUILD_ARG_MODE=normal"

call :ParseArgs %*
if errorlevel 1 (
    echo.
    endlocal
    pause
    exit /b 1
)

if "%HELP_ONLY%"=="1" (
    echo.
    endlocal
    pause
    exit /b 0
)

if "%CHECK_ONLY%"=="0" if "%COMMIT_AFTER_BUILD%"=="1" (
    call :AskBuildReason
    if errorlevel 1 (
        echo.
        endlocal
        pause
        exit /b 1
    )
)

set "APP_NAME=window"
set "EXE_NAME=window.exe"

set "TIME_FORMAT=yyyy-MM-dd.HH'h'mm's'ss"
set "LIBS=-luser32 -lgdi32 -lcomctl32 -lgdiplus -lole32"
set "TCC_FLAGS=-mwindows"

set "THEME_ROOT=themes"
set "THEME_GENERATED_C=theme_resources_generated.c"
set "THEME_HASH_FILE=.theme_resources.hash"
set "THEME_HASH_NEW_FILE=.theme_resources.hash.new"

set "EMBEDDED_RESOURCE_GENERATOR=generate_embedded_resources.ps1"
set "EMBEDDED_GENERATED_C=embedded_resources_generated.c"
set "EMBEDDED_HASH_FILE=.embedded_resources.hash"
set "EMBEDDED_HASH_NEW_FILE=.embedded_resources.hash.new"

set "TEMP_ROOT=temp"

set "BUILD_COMPLETE=0"
set "CREATED_FINAL_BUILD=0"
set "CREATED_FINAL_SOURCE=0"

call :Step "Build requested"

if "%CHECK_ONLY%"=="1" (
    call :Warn "Check mode: no compile, no build folders, no source snapshot, no commit, no push."
) else (
    if "%COMMIT_AFTER_BUILD%"=="1" (
        call :Info "Purpose: !BUILD_REASON!"
    ) else (
        call :Warn "nosync mode: build only, no commit, no push."
    )

    if "%PUSH_AFTER_BUILD%"=="0" if "%COMMIT_AFTER_BUILD%"=="1" call :Warn "nopush mode: will commit locally but will not push to GitHub."
)

call :SelectTCC
if errorlevel 1 goto fail

call :RequireCommand powershell "PowerShell was not found in PATH."
if errorlevel 1 goto fail

call :RequireCommand robocopy "Robocopy was not found in PATH."
if errorlevel 1 goto fail

if "%CHECK_ONLY%"=="1" (
    call :RunCheckOnly
    if errorlevel 1 goto fail

    call :Line
    call :Ok "Build check passed."
    call :Info "Selected compiler: !TCC_EXE!"
    if exist ".git" (
        echo.
        git status -sb
    )
    call :Line
    echo.
    endlocal
    pause
    exit /b 0
)

for /f %%A in ('powershell -NoProfile -Command "Get-Date -Format \"%TIME_FORMAT%\""') do set "BUILD_STAMP=%%A"

if not defined BUILD_STAMP (
    set "FAIL_MESSAGE=Could not create build timestamp."
    goto fail
)

set "BUILD_DIR=build_%BUILD_STAMP%"
set "SOURCE_DIR=source_%BUILD_STAMP%"
set "OLDBUILDS_DIR=oldbuilds"

set "WORK_BUILD_DIR=%TEMP_ROOT%\build_work_%BUILD_STAMP%"
set "WORK_SOURCE_DIR=%TEMP_ROOT%\source_work_%BUILD_STAMP%"
set "THEME_FILTERED_ROOT=%TEMP_ROOT%\theme_resource_input_%BUILD_STAMP%"

set "WORK_OUTPUT_EXE=%WORK_BUILD_DIR%\%EXE_NAME%"
set "FINAL_OUTPUT_EXE=%BUILD_DIR%\%EXE_NAME%"
set "CURRENT_HARDLINK=%EXE_NAME%"

call :Step "Checking embedded theme resource source"

powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; $r='%THEME_ROOT%'; $g='generate_theme_resources.ps1'; $o='%THEME_HASH_NEW_FILE%'; $a=@(); $skip={ param($rel) $parts=$rel -split '[\\/]' ; foreach($p in $parts){ if($p -like 'build_*' -or $p -like 'source_*' -or $p -ieq 'oldbuilds' -or $p -ieq 'temp' -or $p -ieq 'tcc'){ return $true } } return $false }; if(Test-Path $g){$a += ('GEN ' + (Get-FileHash $g).Hash)} else {$a += 'GEN_MISSING'}; if(Test-Path $r){$b=(Resolve-Path $r).Path; Get-ChildItem $r -File -Recurse | Sort-Object FullName | ForEach-Object { $rel=$_.FullName.Substring($b.Length).TrimStart('\','/').Replace('\','/'); if(-not (& $skip $rel)){ $a += ('FILE ' + $rel + ' ' + (Get-FileHash $_.FullName).Hash) } }} else {$a += 'THEME_ROOT_MISSING'}; $a | Set-Content -Encoding ASCII $o"
call :CheckError "Theme hash check failed."

set "REBUILD_THEME_RESOURCES=0"
if not exist "%THEME_GENERATED_C%" set "REBUILD_THEME_RESOURCES=1"
if not exist "%THEME_HASH_FILE%" set "REBUILD_THEME_RESOURCES=1"
if exist "%THEME_HASH_FILE%" if exist "%THEME_HASH_NEW_FILE%" fc /b "%THEME_HASH_FILE%" "%THEME_HASH_NEW_FILE%" >nul || set "REBUILD_THEME_RESOURCES=1"

if "%REBUILD_THEME_RESOURCES%"=="1" (
    call :Info "Preparing filtered theme resource input..."

    powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; $src='%THEME_ROOT%'; $dst='%THEME_FILTERED_ROOT%'; $skip={ param($rel) $parts=$rel -split '[\\/]' ; foreach($p in $parts){ if($p -like 'build_*' -or $p -like 'source_*' -or $p -ieq 'oldbuilds' -or $p -ieq 'temp' -or $p -ieq 'tcc'){ return $true } } return $false }; if(Test-Path $dst){Remove-Item $dst -Recurse -Force}; New-Item -ItemType Directory -Force -Path $dst | Out-Null; if(Test-Path $src){$base=(Resolve-Path $src).Path; Get-ChildItem $src -File -Recurse | ForEach-Object { $rel=$_.FullName.Substring($base.Length).TrimStart('\','/'); if(-not (& $skip $rel)){ $target=Join-Path $dst $rel; New-Item -ItemType Directory -Force -Path (Split-Path $target) | Out-Null; Copy-Item $_.FullName $target -Force } }}"
    call :CheckError "Theme resource staging failed."

    call :Info "Generating embedded theme resource source..."

    powershell -NoProfile -ExecutionPolicy Bypass -File generate_theme_resources.ps1 -ThemeRoot "%THEME_FILTERED_ROOT%" -OutFile "%THEME_GENERATED_C%"
    call :CheckError "Theme resource generation failed."

    move /Y "%THEME_HASH_NEW_FILE%" "%THEME_HASH_FILE%" >nul
    call :CheckError "Theme hash update failed."

    call :Ok "Embedded theme resource source regenerated."
) else (
    call :Ok "Embedded theme resource source is up to date."
    if exist "%THEME_HASH_NEW_FILE%" del "%THEME_HASH_NEW_FILE%" >nul 2>nul
)

if exist "%THEME_FILTERED_ROOT%" rmdir /S /Q "%THEME_FILTERED_ROOT%" >nul 2>nul

if exist "ini_resources_generated.c" del "ini_resources_generated.c" >nul 2>nul

call :Step "Checking embedded INI resource source"

powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; $r='.'; $g='%EMBEDDED_RESOURCE_GENERATOR%'; $o='%EMBEDDED_HASH_NEW_FILE%'; $a=@(); $skip={ param($rel) $parts=$rel -split '[\\/]' ; foreach($p in $parts){ if($p -like 'build_*' -or $p -like 'source_*' -or $p -ieq 'oldbuilds' -or $p -ieq 'temp' -or $p -ieq 'tcc'){ return $true } } return $false }; if(Test-Path $g){$a += ('GEN ' + (Get-FileHash $g).Hash)} else {$a += 'GEN_MISSING'}; if(Test-Path $r){$b=(Resolve-Path $r).Path; Get-ChildItem $r -Filter '*.ini' -File -Recurse | Sort-Object FullName | ForEach-Object { $rel=$_.FullName.Substring($b.Length).TrimStart('\','/').Replace('\','/'); if(-not (& $skip $rel)){ $a += ('FILE ' + $rel + ' ' + (Get-FileHash $_.FullName).Hash) } }} else {$a += 'ROOT_MISSING'}; $a | Set-Content -Encoding ASCII $o"
call :CheckError "Embedded resource hash check failed."

set "REBUILD_EMBEDDED_RESOURCES=0"
if not exist "%EMBEDDED_GENERATED_C%" set "REBUILD_EMBEDDED_RESOURCES=1"
if not exist "%EMBEDDED_HASH_FILE%" set "REBUILD_EMBEDDED_RESOURCES=1"
if exist "%EMBEDDED_HASH_FILE%" if exist "%EMBEDDED_HASH_NEW_FILE%" fc /b "%EMBEDDED_HASH_FILE%" "%EMBEDDED_HASH_NEW_FILE%" >nul || set "REBUILD_EMBEDDED_RESOURCES=1"

if "%REBUILD_EMBEDDED_RESOURCES%"=="1" (
    call :Info "Generating embedded INI resource source..."

    powershell -NoProfile -ExecutionPolicy Bypass -File "%EMBEDDED_RESOURCE_GENERATOR%" -Root "." -OutFile "%EMBEDDED_GENERATED_C%" -Group "ini" -Include "*.ini"
    call :CheckError "Embedded resource generation failed."

    move /Y "%EMBEDDED_HASH_NEW_FILE%" "%EMBEDDED_HASH_FILE%" >nul
    call :CheckError "Embedded hash update failed."

    call :Ok "Embedded INI resource source regenerated."
) else (
    call :Ok "Embedded INI resource source is up to date."
    if exist "%EMBEDDED_HASH_NEW_FILE%" del "%EMBEDDED_HASH_NEW_FILE%" >nul 2>nul
)

call :Step "Collecting C source files"

set "CFILES="
for %%F in (*.c) do set CFILES=!CFILES! "%%F"

if not defined CFILES (
    set "FAIL_MESSAGE=No .c files were found."
    goto fail
)

call :Ok "C source files found."

call :Step "Preparing temporary build folders"

if not exist "%TEMP_ROOT%" mkdir "%TEMP_ROOT%" >nul 2>nul
if not exist "%TEMP_ROOT%" (
    set "FAIL_MESSAGE=Could not create temp folder."
    goto fail
)

if exist "%WORK_BUILD_DIR%" rmdir /S /Q "%WORK_BUILD_DIR%" >nul 2>nul
if exist "%WORK_SOURCE_DIR%" rmdir /S /Q "%WORK_SOURCE_DIR%" >nul 2>nul

mkdir "%WORK_BUILD_DIR%" >nul 2>nul
if not exist "%WORK_BUILD_DIR%" (
    set "FAIL_MESSAGE=Could not create temporary build folder."
    goto fail
)

call :Ok "Temporary folders ready."

call :Step "Compiling"

call :Info "Compiler: !TCC_EXE!"
call :Info "Output:   %WORK_OUTPUT_EXE%"

"!TCC_EXE!" %TCC_FLAGS% -o "%WORK_OUTPUT_EXE%" !CFILES! %LIBS%
call :CheckError "Build failed."

if not exist "%WORK_OUTPUT_EXE%" (
    set "FAIL_MESSAGE=Build command completed, but output exe was not created."
    goto fail
)

call :Ok "Compilation succeeded."

call :Step "Creating source snapshot"

robocopy . "%WORK_SOURCE_DIR%" /E /XD .git build_* source_* oldbuilds temp tcc /XF "%EXE_NAME%" *.exe *.obj *.pdb *.ilk *.res *.log >nul
call :CheckRobocopy "Source copy failed."

call :Ok "Source snapshot prepared."

call :Step "Copying source snapshot into build folder"

robocopy "%WORK_SOURCE_DIR%" "%WORK_BUILD_DIR%\%SOURCE_DIR%" /E >nul
call :CheckRobocopy "Build source copy failed."

call :Ok "Build source snapshot prepared."

call :Step "Promoting successful build"

if exist "%BUILD_DIR%" (
    set "FAIL_MESSAGE=Final build folder already exists: %BUILD_DIR%"
    goto fail
)

if exist "%SOURCE_DIR%" (
    set "FAIL_MESSAGE=Final source folder already exists: %SOURCE_DIR%"
    goto fail
)

move "%WORK_BUILD_DIR%" "%BUILD_DIR%" >nul
call :CheckError "Could not move temporary build folder to final build folder."
set "CREATED_FINAL_BUILD=1"

move "%WORK_SOURCE_DIR%" "%SOURCE_DIR%" >nul
call :CheckError "Could not move temporary source folder to final source folder."
set "CREATED_FINAL_SOURCE=1"

set "BUILD_COMPLETE=1"

call :Ok "Final build and source folders created."

call :Step "Archiving older build/source folders"

if not exist "%OLDBUILDS_DIR%" mkdir "%OLDBUILDS_DIR%" >nul 2>nul

if exist "%OLDBUILDS_DIR%" (
    for /d %%D in (build_*) do if /i not "%%~nxD"=="%BUILD_DIR%" move "%%D" "%OLDBUILDS_DIR%\%%~nxD" >nul 2>nul
    for /d %%D in (source_*) do if /i not "%%~nxD"=="%SOURCE_DIR%" move "%%D" "%OLDBUILDS_DIR%\%%~nxD" >nul 2>nul
    call :Ok "Older build/source folders archived."
) else (
    call :Warn "Could not create oldbuilds folder. Build still succeeded."
)

call :Step "Updating current exe link"

if exist "%CURRENT_HARDLINK%" del "%CURRENT_HARDLINK%" >nul 2>nul

mklink /H "%CURRENT_HARDLINK%" "%FINAL_OUTPUT_EXE%" >nul 2>nul
if errorlevel 1 copy /Y "%FINAL_OUTPUT_EXE%" "%CURRENT_HARDLINK%" >nul 2>nul

if exist "%CURRENT_HARDLINK%" (
    call :Ok "Updated %CURRENT_HARDLINK%."
) else (
    call :Warn "Build succeeded, but could not update %CURRENT_HARDLINK%."
)

call :CleanupTemp

call :Line
call :Ok "Build complete."
call :Info "Output: %FINAL_OUTPUT_EXE%"
call :Info "Link:   %CURRENT_HARDLINK%"
call :Line

if "%COMMIT_AFTER_BUILD%"=="1" (
    call :GitSync
    if errorlevel 1 call :Warn "Build succeeded, but GitHub sync failed."
) else (
    call :Warn "Git commit/push skipped by command-line argument."
)

echo.
endlocal
pause
exit /b 0

:ParseArgs
if "%~1"=="" exit /b 0

if /I "%~1"=="help" (
    if /I "%~2"=="howtouse" (
        call :ShowBuildHowToUse
        set "HELP_ONLY=1"
        exit /b 0
    )

    call :ShowBuildArgsHelp
    set "HELP_ONLY=1"
    exit /b 0
)

if /I "%~1"=="/help" (
    if /I "%~2"=="howtouse" (
        call :ShowBuildHowToUse
        set "HELP_ONLY=1"
        exit /b 0
    )

    call :ShowBuildArgsHelp
    set "HELP_ONLY=1"
    exit /b 0
)

if /I "%~1"=="--help" (
    call :ShowBuildArgsHelp
    set "HELP_ONLY=1"
    exit /b 0
)

if /I "%~1"=="-h" (
    call :ShowBuildArgsHelp
    set "HELP_ONLY=1"
    exit /b 0
)

if /I "%~1"=="/?" (
    call :ShowBuildArgsHelp
    set "HELP_ONLY=1"
    exit /b 0
)

if /I "%~1"=="howtouse" (
    call :ShowBuildHowToUse
    set "HELP_ONLY=1"
    exit /b 0
)

if /I "%~1"=="nopush" (
    set "PUSH_AFTER_BUILD=0"
    set "BUILD_ARG_MODE=nopush"
    shift
    goto ParseArgs
)

if /I "%~1"=="/nopush" (
    set "PUSH_AFTER_BUILD=0"
    set "BUILD_ARG_MODE=nopush"
    shift
    goto ParseArgs
)

if /I "%~1"=="nosync" (
    set "COMMIT_AFTER_BUILD=0"
    set "PUSH_AFTER_BUILD=0"
    set "BUILD_ARG_MODE=nosync"
    shift
    goto ParseArgs
)

if /I "%~1"=="/nosync" (
    set "COMMIT_AFTER_BUILD=0"
    set "PUSH_AFTER_BUILD=0"
    set "BUILD_ARG_MODE=nosync"
    shift
    goto ParseArgs
)

if /I "%~1"=="check" (
    set "CHECK_ONLY=1"
    set "COMMIT_AFTER_BUILD=0"
    set "PUSH_AFTER_BUILD=0"
    set "BUILD_ARG_MODE=check"
    shift
    goto ParseArgs
)

if /I "%~1"=="/check" (
    set "CHECK_ONLY=1"
    set "COMMIT_AFTER_BUILD=0"
    set "PUSH_AFTER_BUILD=0"
    set "BUILD_ARG_MODE=check"
    shift
    goto ParseArgs
)

if /I "%~1"=="test" (
    set "CHECK_ONLY=1"
    set "COMMIT_AFTER_BUILD=0"
    set "PUSH_AFTER_BUILD=0"
    set "BUILD_ARG_MODE=test"
    shift
    goto ParseArgs
)

if /I "%~1"=="/test" (
    set "CHECK_ONLY=1"
    set "COMMIT_AFTER_BUILD=0"
    set "PUSH_AFTER_BUILD=0"
    set "BUILD_ARG_MODE=test"
    shift
    goto ParseArgs
)

call :FailText "Unknown build argument: %~1"
echo.
call :ShowBuildArgsHelp
exit /b 1

:ShowBuildArgsHelp
call :Line
call :Say BFT.color.header "build.bat command-line options"
call :Line
echo(
echo   build.bat
echo     Normal build.
echo     Asks what the build is for.
echo     If successful: commit and push to GitHub.
echo(
echo   build.bat nopush
echo     Builds normally.
echo     Commits successful changes locally.
echo     Does not push to GitHub.
echo(
echo   build.bat nosync
echo     Builds normally.
echo     Does not ask for a commit message.
echo     Does not commit and does not push.
echo(
echo   build.bat check
echo     Checks build prerequisites.
echo     Finds or downloads TCC if needed.
echo     Does not compile, snapshot, commit, or push.
echo(
echo   build.bat test
echo     Same as check.
echo     Useful before trying a real build.
echo     Does not change the project.
echo(
echo   build.bat help
echo     Shows this command list.
echo(
echo   build.bat help howtouse
echo     Explains how build.bat works.
echo     Shows the normal workflow.
echo     Explains what gets committed and pushed.
echo(
call :Line
echo(
echo Common choices:
echo   build.bat                 Normal build, commit, push
echo   build.bat nopush          Build and commit, but push later
echo   build.bat nosync          Build only
echo   build.bat check           Check setup only
echo   build.bat help howtouse   Detailed explanation
echo(
exit /b 0

:ShowBuildHowToUse
call :Line
call :Say BFT.color.header "How build.bat works"
call :Line
echo(
echo Normal use:
echo   1. Edit your source files.
echo   2. Run:
echo        build.bat
echo   3. Type what the build is for.
echo   4. If the build succeeds, it commits and pushes to GitHub.
echo(
echo Build purpose:
echo   Normal build and nopush mode ask for a build purpose.
echo   That text becomes the Git commit message.
echo   Empty messages are rejected.
echo(
echo To cancel at the build purpose prompt:
echo   Press Ctrl+C.
echo   The build should stop before compiling.
echo(
echo Compiler selection:
echo   build.bat prefers:
echo        .\tcc\tcc.exe
echo   If that does not exist, it tries:
echo        tcc.exe from PATH
echo   If neither exists, it runs:
echo        gettcc.bat
echo(
echo Resource generation:
echo   Theme resources are regenerated only when theme files changed.
echo   Embedded INI resources are regenerated only when INI files changed.
echo   The actual program still builds each time you run build.bat.
echo(
echo Build folders:
echo   It first builds into temp folders.
echo   It creates final build_... and source_... folders only after success.
echo   Failed builds should not leave empty final build/source folders.
echo(
echo GitHub sync:
echo   Successful normal builds run:
echo        git add --all
echo        git commit
echo        git push
echo(
echo What GitHub receives:
echo   GitHub receives tracked source files and project files.
echo   GitHub does not receive ignored build folders, oldbuilds, temp, tcc, or exe files.
echo(
echo Useful modes:
echo   build.bat
echo     Build, commit, and push.
echo(
echo   build.bat nopush
echo     Build and commit locally, but do not push.
echo     Use this when offline or when you want to push later.
echo(
echo   build.bat nosync
echo     Build only.
echo     No commit and no push.
echo     Use this for experiments you do not want saved to Git yet.
echo(
echo   build.bat check
echo     Check prerequisites only.
echo     No compile, no snapshot, no commit, no push.
echo(
call :Line
echo(
exit /b 0

:AskBuildReason
call :Line
call :Say BFT.color.header "FindReplaceMark build"
call :Say BFT.color.note "Type what this build is for. This becomes the Git commit message after a successful build."
call :Say BFT.color.skip "Press Ctrl+C to cancel before anything builds."
call :Say BFT.color.skip "Empty messages are not accepted."
call :Line

:AskBuildReasonLoop
set "BUILD_REASON="
set "BUILD_REASON_STRIPPED="
set "BUILD_REASON_FILE=%TEMP%\npp-findreplace-build-reason-%RANDOM%-%RANDOM%.txt"

echo.
call :PrintWord BFT.color.cyan "Build purpose"
<nul set /p "= : "

powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; try { [Console]::TreatControlCAsInput=$false; $s=[Console]::ReadLine(); if($null -eq $s){ exit 130 }; $s=$s.Trim(); if([string]::IsNullOrWhiteSpace($s)){ exit 3 }; $s=$s.Replace([char]34,[char]39).Replace([char]33,'.'); [IO.File]::WriteAllText($env:BUILD_REASON_FILE, $s, [Text.Encoding]::Default); exit 0 } catch { exit 130 }"

set "ASK_REASON_RC=%ERRORLEVEL%"

if exist "%BUILD_REASON_FILE%" (
    set /p "BUILD_REASON="<"%BUILD_REASON_FILE%"
    del "%BUILD_REASON_FILE%" >nul 2>nul
)

if "%ASK_REASON_RC%"=="0" (
    set "BUILD_REASON_STRIPPED=%BUILD_REASON: =%"

    if defined BUILD_REASON_STRIPPED (
        exit /b 0
    )

    call :Say BFT.color.fail "A build purpose is required. Press Ctrl+C to cancel."
    goto AskBuildReasonLoop
)

if "%ASK_REASON_RC%"=="3" (
    call :Say BFT.color.fail "A build purpose is required. Press Ctrl+C to cancel."
    goto AskBuildReasonLoop
)

echo.
call :Warn "Build cancelled before anything was built."
exit /b 130

:FindTCC
set "TCC_EXE="

if exist ".\tcc\tcc.exe" (
    set "TCC_EXE=.\tcc\tcc.exe"
    exit /b 0
)

for %%T in (tcc.exe) do set "TCC_EXE=%%~$PATH:T"

if defined TCC_EXE exit /b 0

exit /b 1

:SelectTCC
call :Step "Selecting Tiny C Compiler"

call :FindTCC
if not errorlevel 1 (
    if /I "!TCC_EXE!"==".\tcc\tcc.exe" (
        call :Ok "Using bundled Tiny C Compiler: .\tcc\tcc.exe"
    ) else (
        call :Ok "Using Tiny C Compiler from PATH: !TCC_EXE!"
    )
    exit /b 0
)

call :Warn "Tiny C Compiler was not found."

if exist "gettcc.bat" (
    call :Info "Running gettcc.bat..."
    call gettcc.bat
    cd /d "%~dp0"

    if errorlevel 1 (
        set "FAIL_MESSAGE=gettcc.bat failed."
        exit /b 1
    )
) else (
    set "FAIL_MESSAGE=Tiny C Compiler was not found, and gettcc.bat does not exist."
    exit /b 1
)

call :FindTCC
if errorlevel 1 (
    set "FAIL_MESSAGE=gettcc.bat ran, but Tiny C Compiler is still unavailable."
    exit /b 1
)

if /I "!TCC_EXE!"==".\tcc\tcc.exe" (
    call :Ok "Using bundled Tiny C Compiler after gettcc.bat: .\tcc\tcc.exe"
) else (
    call :Ok "Using Tiny C Compiler after gettcc.bat: !TCC_EXE!"
)

exit /b 0

:RequireCommand
where %~1 >nul 2>nul
if errorlevel 1 (
    set "FAIL_MESSAGE=%~2"
    exit /b 1
)
exit /b 0

:RunCheckOnly
set "CHECK_FAILED=0"

call :Step "Checking project files"

if exist "generate_theme_resources.ps1" (
    call :Ok "Found generate_theme_resources.ps1."
) else (
    call :FailText "Missing generate_theme_resources.ps1."
    set "CHECK_FAILED=1"
)

if exist "%EMBEDDED_RESOURCE_GENERATOR%" (
    call :Ok "Found %EMBEDDED_RESOURCE_GENERATOR%."
) else (
    call :FailText "Missing %EMBEDDED_RESOURCE_GENERATOR%."
    set "CHECK_FAILED=1"
)

if exist "%THEME_ROOT%" (
    call :Ok "Found themes folder."
) else (
    call :Warn "Themes folder was not found."
)

set "CHECK_CFILES="
for %%F in (*.c) do set CHECK_CFILES=1

if defined CHECK_CFILES (
    call :Ok "Found .c source files."
) else (
    call :FailText "No .c source files were found."
    set "CHECK_FAILED=1"
)

if exist ".git" (
    call :Ok "Git repository detected."
) else (
    call :Warn "This folder is not a Git repository. Build can run, but GitHub sync will be skipped."
)

where git >nul 2>nul
if errorlevel 1 (
    call :Warn "git was not found. Build can run, but GitHub sync will fail."
) else (
    call :Ok "git found."
)

if "%CHECK_FAILED%"=="1" (
    set "FAIL_MESSAGE=Build check found problems."
    exit /b 1
)

exit /b 0

:GitSync
call :Step "GitHub sync"

if not exist ".git" (
    call :Warn "Git sync skipped: this folder is not a Git repository."
    exit /b 0
)

where git >nul 2>nul
if errorlevel 1 (
    call :Warn "Git sync skipped: git was not found."
    exit /b 1
)

call :EnsureGitIgnoreSafety

set "GIT_DIRTY="
for /f "delims=" %%G in ('git status --porcelain') do set "GIT_DIRTY=1"

if defined GIT_DIRTY (
    call :Info "Successful build. These files changed:"
    echo.
    git status --short
    echo.

    call :Info "Commit message:"
    call :Say BFT.color.summary "!BUILD_REASON!"
    echo.

    git add --all
    if errorlevel 1 (
        call :FailText "git add failed."
        exit /b 1
    )

    set "COMMIT_MSG_FILE=%TEMP%\npp-findreplace-build-commit-%RANDOM%-%RANDOM%.txt"

    powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; Set-Content -LiteralPath $env:COMMIT_MSG_FILE -Value $env:BUILD_REASON -Encoding UTF8"
    if errorlevel 1 (
        call :FailText "Could not create temporary commit message file."
        exit /b 1
    )

    git commit -F "%COMMIT_MSG_FILE%"
    set "GIT_COMMIT_RC=%ERRORLEVEL%"

    if exist "%COMMIT_MSG_FILE%" del "%COMMIT_MSG_FILE%" >nul 2>nul

    if not "%GIT_COMMIT_RC%"=="0" (
        call :FailText "git commit failed."
        exit /b 1
    )
) else (
    call :Ok "No Git changes to commit."
)

if "%PUSH_AFTER_BUILD%"=="1" (
    call :Info "Pushing to GitHub..."

    git push
    if errorlevel 1 (
        call :FailText "git push failed."
        exit /b 1
    )

    call :Ok "GitHub sync complete."
) else (
    call :Warn "Push skipped because nopush mode is active."
    call :Info "Later, push local commits with:"
    echo   git-push-local.bat
)

exit /b 0

:EnsureGitIgnoreSafety
if not exist ".gitignore" type nul > ".gitignore"

call :EnsureGitIgnoreLine "build_*/"
call :EnsureGitIgnoreLine "source_*/"
call :EnsureGitIgnoreLine "oldbuilds/"
call :EnsureGitIgnoreLine "oldbuild/"
call :EnsureGitIgnoreLine "temp/"
call :EnsureGitIgnoreLine "tcc/"
call :EnsureGitIgnoreLine "*.exe"
call :EnsureGitIgnoreLine "*.obj"
call :EnsureGitIgnoreLine "*.pdb"
call :EnsureGitIgnoreLine "*.ilk"
call :EnsureGitIgnoreLine "*.res"
call :EnsureGitIgnoreLine "*.log"

exit /b 0

:EnsureGitIgnoreLine
set "GITIGNORE_LINE=%~1"

findstr /L /X /C:"%GITIGNORE_LINE%" ".gitignore" >nul 2>nul
if errorlevel 1 (
    >> ".gitignore" echo(%GITIGNORE_LINE%
)

exit /b 0

:CheckError
if errorlevel 1 (
    set "FAIL_MESSAGE=%~1"
    goto fail
)
exit /b 0

:CheckRobocopy
if errorlevel 8 (
    set "FAIL_MESSAGE=%~1"
    goto fail
)
exit /b 0

:CleanupTemp
if defined WORK_BUILD_DIR if exist "%WORK_BUILD_DIR%" rmdir /S /Q "%WORK_BUILD_DIR%" >nul 2>nul
if defined WORK_SOURCE_DIR if exist "%WORK_SOURCE_DIR%" rmdir /S /Q "%WORK_SOURCE_DIR%" >nul 2>nul
if defined THEME_FILTERED_ROOT if exist "%THEME_FILTERED_ROOT%" rmdir /S /Q "%THEME_FILTERED_ROOT%" >nul 2>nul

if exist "%THEME_HASH_NEW_FILE%" del "%THEME_HASH_NEW_FILE%" >nul 2>nul
if exist "%EMBEDDED_HASH_NEW_FILE%" del "%EMBEDDED_HASH_NEW_FILE%" >nul 2>nul

if not "%BUILD_COMPLETE%"=="1" (
    if "%CREATED_FINAL_BUILD%"=="1" if defined BUILD_DIR if exist "%BUILD_DIR%" rmdir /S /Q "%BUILD_DIR%" >nul 2>nul
    if "%CREATED_FINAL_SOURCE%"=="1" if defined SOURCE_DIR if exist "%SOURCE_DIR%" rmdir /S /Q "%SOURCE_DIR%" >nul 2>nul
)

if defined TEMP_ROOT rmdir "%TEMP_ROOT%" >nul 2>nul
exit /b 0

:fail
echo.
if defined FAIL_MESSAGE (
    call :FailText "%FAIL_MESSAGE%"
) else (
    call :FailText "Build failed."
)
call :CleanupTemp
echo.
endlocal
pause
exit /b 1

:Line
echo(!BFT.color.gray!------------------------------------------------------------!BFT.color.reset!
exit /b 0

:Step
echo.
echo(!BFT.color.header!== %~1 ==!BFT.color.reset!
exit /b 0

:Info
echo(!BFT.color.white!%~1!BFT.color.reset!
exit /b 0

:Ok
echo(!BFT.color.pass!OK: %~1!BFT.color.reset!
exit /b 0

:Warn
echo(!BFT.color.skip!WARNING: %~1!BFT.color.reset!
exit /b 0

:FailText
echo(!BFT.color.fail!ERROR: %~1!BFT.color.reset!
exit /b 0

:Say
set "say_colour=!%~1!"
echo(!say_colour!%~2!BFT.color.reset!
exit /b 0

:PrintWord
set "word_colour=!%~1!"
<nul set /p "=!word_colour!%~2!BFT.color.reset!"
exit /b 0

:SetNoColours
set "BFT.esc="
set "BFT.colour.black="
set "BFT.colour.red="
set "BFT.colour.green="
set "BFT.colour.yellow="
set "BFT.colour.blue="
set "BFT.colour.magenta="
set "BFT.colour.cyan="
set "BFT.colour.white="
set "BFT.colour.gray="
set "BFT.colour.bold="
set "BFT.colour.clear="
set "BFT.colour.reset="
set "BFT.colour.pass="
set "BFT.colour.fail="
set "BFT.colour.skip="
set "BFT.colour.harnessfail="
set "BFT.colour.header="
set "BFT.colour.summary="
set "BFT.colour.note="
set "BFT.color.black="
set "BFT.color.red="
set "BFT.color.green="
set "BFT.color.yellow="
set "BFT.color.blue="
set "BFT.color.magenta="
set "BFT.color.cyan="
set "BFT.color.white="
set "BFT.color.gray="
set "BFT.color.bold="
set "BFT.color.clear="
set "BFT.color.reset="
set "BFT.color.pass="
set "BFT.color.fail="
set "BFT.color.skip="
set "BFT.color.harnessfail="
set "BFT.color.header="
set "BFT.color.summary="
set "BFT.color.note="
exit /b 0

:SetESC
for /f "tokens=1 delims==" %%v in ('set sesc_ 2^>nul') do set "%%v="
if defined _sesc_rc (set "_sesc_rc=" & exit /b %_sesc_rc%)
set "sesc_out=%~1"
if not defined sesc_out ( set "_sesc_rc=2" & goto :SetESC )
for /f %%a in ('echo prompt $E^| cmd') do set "%sesc_out%=%%a"
set "_sesc_rc=0" & goto :SetESC

:SetBFTColours
if not defined BFT.esc call :SetESC BFT.esc
if not defined BFT.esc exit /b 1
set "BFT.colour.black=%BFT.esc%[30m"
set "BFT.colour.red=%BFT.esc%[31m"
set "BFT.colour.green=%BFT.esc%[32m"
set "BFT.colour.yellow=%BFT.esc%[33m"
set "BFT.colour.blue=%BFT.esc%[34m"
set "BFT.colour.magenta=%BFT.esc%[35m"
set "BFT.colour.cyan=%BFT.esc%[36m"
set "BFT.colour.white=%BFT.esc%[37m"
set "BFT.colour.gray=%BFT.esc%[90m"
set "BFT.colour.bold=%BFT.esc%[1m"
set "BFT.colour.clear=%BFT.esc%[0m"
set "BFT.colour.reset=%BFT.colour.clear%"
set "BFT.colour.pass=%BFT.colour.green%"
set "BFT.colour.fail=%BFT.colour.red%"
set "BFT.colour.skip=%BFT.colour.yellow%"
set "BFT.colour.harnessfail=%BFT.colour.magenta%"
set "BFT.colour.header=%BFT.colour.cyan%"
set "BFT.colour.summary=%BFT.colour.bold%"
set "BFT.colour.note=%BFT.colour.gray%"
set "BFT.color.black=%BFT.colour.black%"
set "BFT.color.red=%BFT.colour.red%"
set "BFT.color.green=%BFT.colour.green%"
set "BFT.color.yellow=%BFT.colour.yellow%"
set "BFT.color.blue=%BFT.colour.blue%"
set "BFT.color.magenta=%BFT.colour.magenta%"
set "BFT.color.cyan=%BFT.colour.cyan%"
set "BFT.color.white=%BFT.colour.white%"
set "BFT.color.gray=%BFT.colour.gray%"
set "BFT.color.bold=%BFT.colour.bold%"
set "BFT.color.clear=%BFT.colour.clear%"
set "BFT.color.reset=%BFT.colour.reset%"
set "BFT.color.pass=%BFT.colour.pass%"
set "BFT.color.fail=%BFT.colour.fail%"
set "BFT.color.skip=%BFT.colour.skip%"
set "BFT.color.harnessfail=%BFT.colour.harnessfail%"
set "BFT.color.header=%BFT.colour.header%"
set "BFT.color.summary=%BFT.colour.summary%"
set "BFT.color.note=%BFT.colour.note%"
exit /b 0