@echo off
setlocal EnableExtensions EnableDelayedExpansion

set APP_NAME=window
set EXE_NAME=window.exe

set TIME_FORMAT=yyyy-MM-dd.HH'h'mm's'ss
set LIBS=-luser32 -lgdi32 -lcomctl32 -lgdiplus -lole32
set TCC_FLAGS=-mwindows

set THEME_ROOT=themes
set THEME_GENERATED_C=theme_resources_generated.c
set THEME_HASH_FILE=.theme_resources.hash
set THEME_HASH_NEW_FILE=.theme_resources.hash.new

set EMBEDDED_RESOURCE_GENERATOR=generate_embedded_resources.ps1
set EMBEDDED_GENERATED_C=embedded_resources_generated.c
set EMBEDDED_HASH_FILE=.embedded_resources.hash
set EMBEDDED_HASH_NEW_FILE=.embedded_resources.hash.new

set TEMP_ROOT=temp
set THEME_FILTERED_ROOT=%TEMP_ROOT%\theme_resource_input

for /f %%A in ('powershell -NoProfile -Command "Get-Date -Format \"%TIME_FORMAT%\""') do set BUILD_STAMP=%%A

set BUILD_DIR=build_%BUILD_STAMP%
set SOURCE_DIR=source_%BUILD_STAMP%
set OLDBUILDS_DIR=oldbuilds
set OUTPUT_EXE=%BUILD_DIR%\%EXE_NAME%
set CURRENT_HARDLINK=%EXE_NAME%

if not exist "%OLDBUILDS_DIR%" mkdir "%OLDBUILDS_DIR%"

for /d %%D in (build_*) do if /i not "%%~nxD"=="%BUILD_DIR%" move "%%D" "%OLDBUILDS_DIR%\%%~nxD" >nul
for /d %%D in (source_*) do if /i not "%%~nxD"=="%SOURCE_DIR%" move "%%D" "%OLDBUILDS_DIR%\%%~nxD" >nul

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%SOURCE_DIR%" mkdir "%SOURCE_DIR%"
if not exist "%TEMP_ROOT%" mkdir "%TEMP_ROOT%"

echo Checking embedded theme resource source...

powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; $r='%THEME_ROOT%'; $g='generate_theme_resources.ps1'; $o='%THEME_HASH_NEW_FILE%'; $a=@(); $skip={ param($rel) $parts=$rel -split '[\\/]' ; foreach($p in $parts){ if($p -like 'build_*' -or $p -like 'source_*' -or $p -ieq 'oldbuilds' -or $p -ieq 'temp'){ return $true } } return $false }; if(Test-Path $g){$a += ('GEN ' + (Get-FileHash $g).Hash)} else {$a += 'GEN_MISSING'}; if(Test-Path $r){$b=(Resolve-Path $r).Path; Get-ChildItem $r -File -Recurse | Sort-Object FullName | ForEach-Object { $rel=$_.FullName.Substring($b.Length).TrimStart('\','/').Replace('\','/'); if(-not (& $skip $rel)){ $a += ('FILE ' + $rel + ' ' + (Get-FileHash $_.FullName).Hash) } }} else {$a += 'THEME_ROOT_MISSING'}; $a | Set-Content -Encoding ASCII $o"
if errorlevel 1 echo Theme hash check failed. & pause & exit /b 1

set REBUILD_THEME_RESOURCES=0
if not exist "%THEME_GENERATED_C%" set REBUILD_THEME_RESOURCES=1
if not exist "%THEME_HASH_FILE%" set REBUILD_THEME_RESOURCES=1
if exist "%THEME_HASH_FILE%" if exist "%THEME_HASH_NEW_FILE%" fc /b "%THEME_HASH_FILE%" "%THEME_HASH_NEW_FILE%" >nul || set REBUILD_THEME_RESOURCES=1

if "%REBUILD_THEME_RESOURCES%"=="1" echo Preparing filtered theme resource input...
if "%REBUILD_THEME_RESOURCES%"=="1" powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; $src='%THEME_ROOT%'; $dst='%THEME_FILTERED_ROOT%'; $skip={ param($rel) $parts=$rel -split '[\\/]' ; foreach($p in $parts){ if($p -like 'build_*' -or $p -like 'source_*' -or $p -ieq 'oldbuilds' -or $p -ieq 'temp'){ return $true } } return $false }; if(Test-Path $dst){Remove-Item $dst -Recurse -Force}; New-Item -ItemType Directory -Force -Path $dst | Out-Null; if(Test-Path $src){$base=(Resolve-Path $src).Path; Get-ChildItem $src -File -Recurse | ForEach-Object { $rel=$_.FullName.Substring($base.Length).TrimStart('\','/'); if(-not (& $skip $rel)){ $target=Join-Path $dst $rel; New-Item -ItemType Directory -Force -Path (Split-Path $target) | Out-Null; Copy-Item $_.FullName $target -Force } }}"
if errorlevel 1 echo Theme resource staging failed. & pause & exit /b 1

if "%REBUILD_THEME_RESOURCES%"=="1" echo Generating embedded theme resource source...
if "%REBUILD_THEME_RESOURCES%"=="1" powershell -NoProfile -ExecutionPolicy Bypass -File generate_theme_resources.ps1 -ThemeRoot "%THEME_FILTERED_ROOT%" -OutFile "%THEME_GENERATED_C%"
if errorlevel 1 echo Theme resource generation failed. & pause & exit /b 1
if "%REBUILD_THEME_RESOURCES%"=="1" move /Y "%THEME_HASH_NEW_FILE%" "%THEME_HASH_FILE%" >nul
if "%REBUILD_THEME_RESOURCES%"=="1" if exist "%THEME_FILTERED_ROOT%" rmdir /S /Q "%THEME_FILTERED_ROOT%"

if not "%REBUILD_THEME_RESOURCES%"=="1" echo Embedded theme resource source is up to date.
if not "%REBUILD_THEME_RESOURCES%"=="1" if exist "%THEME_HASH_NEW_FILE%" del "%THEME_HASH_NEW_FILE%"

if exist "ini_resources_generated.c" del "ini_resources_generated.c"

echo Checking embedded resource source...

powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; $r='.'; $g='%EMBEDDED_RESOURCE_GENERATOR%'; $o='%EMBEDDED_HASH_NEW_FILE%'; $a=@(); $skip={ param($rel) $parts=$rel -split '[\\/]' ; foreach($p in $parts){ if($p -like 'build_*' -or $p -like 'source_*' -or $p -ieq 'oldbuilds' -or $p -ieq 'temp'){ return $true } } return $false }; if(Test-Path $g){$a += ('GEN ' + (Get-FileHash $g).Hash)} else {$a += 'GEN_MISSING'}; if(Test-Path $r){$b=(Resolve-Path $r).Path; Get-ChildItem $r -Filter '*.ini' -File -Recurse | Sort-Object FullName | ForEach-Object { $rel=$_.FullName.Substring($b.Length).TrimStart('\','/').Replace('\','/'); if(-not (& $skip $rel)){ $a += ('FILE ' + $rel + ' ' + (Get-FileHash $_.FullName).Hash) } }} else {$a += 'ROOT_MISSING'}; $a | Set-Content -Encoding ASCII $o"
if errorlevel 1 echo Embedded resource hash check failed. & pause & exit /b 1

set REBUILD_EMBEDDED_RESOURCES=0
if not exist "%EMBEDDED_GENERATED_C%" set REBUILD_EMBEDDED_RESOURCES=1
if not exist "%EMBEDDED_HASH_FILE%" set REBUILD_EMBEDDED_RESOURCES=1
if exist "%EMBEDDED_HASH_FILE%" if exist "%EMBEDDED_HASH_NEW_FILE%" fc /b "%EMBEDDED_HASH_FILE%" "%EMBEDDED_HASH_NEW_FILE%" >nul || set REBUILD_EMBEDDED_RESOURCES=1

if "%REBUILD_EMBEDDED_RESOURCES%"=="1" echo Generating embedded resource source...
if "%REBUILD_EMBEDDED_RESOURCES%"=="1" powershell -NoProfile -ExecutionPolicy Bypass -File "%EMBEDDED_RESOURCE_GENERATOR%" -Root "." -OutFile "%EMBEDDED_GENERATED_C%" -Group "ini" -Include "*.ini"
if errorlevel 1 echo Embedded resource generation failed. & pause & exit /b 1
if "%REBUILD_EMBEDDED_RESOURCES%"=="1" move /Y "%EMBEDDED_HASH_NEW_FILE%" "%EMBEDDED_HASH_FILE%" >nul

if not "%REBUILD_EMBEDDED_RESOURCES%"=="1" echo Embedded resource source is up to date.
if not "%REBUILD_EMBEDDED_RESOURCES%"=="1" if exist "%EMBEDDED_HASH_NEW_FILE%" del "%EMBEDDED_HASH_NEW_FILE%"

set CFILES=
for %%F in (*.c) do set CFILES=!CFILES! "%%F"

echo Building to %OUTPUT_EXE%...
tcc %TCC_FLAGS% -o "%OUTPUT_EXE%" !CFILES! %LIBS%
if errorlevel 1 echo Build failed. & pause & exit /b 1

echo Copying source snapshot to %SOURCE_DIR%...
robocopy . "%SOURCE_DIR%" /E /XD build_* source_* oldbuilds temp /XF "%EXE_NAME%" >nul
if %ERRORLEVEL% GEQ 8 echo Source copy failed. & pause & exit /b 1

echo Copying source snapshot into %BUILD_DIR%\%SOURCE_DIR%...
robocopy "%SOURCE_DIR%" "%BUILD_DIR%\%SOURCE_DIR%" /E >nul
if %ERRORLEVEL% GEQ 8 echo Build source copy failed. & pause & exit /b 1

if exist "%CURRENT_HARDLINK%" del "%CURRENT_HARDLINK%"
mklink /H "%CURRENT_HARDLINK%" "%OUTPUT_EXE%" >nul
if errorlevel 1 copy /Y "%OUTPUT_EXE%" "%CURRENT_HARDLINK%" >nul

echo Build complete.
echo Output: %OUTPUT_EXE%
echo Link:   %CURRENT_HARDLINK%

endlocal
pause