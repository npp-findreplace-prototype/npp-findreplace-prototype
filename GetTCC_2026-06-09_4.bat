@echo off
setlocal EnableExtensions DisableDelayedExpansion

rem ============================================================
rem GetTCC.bat
rem Standalone Tiny C Compiler bootstrapper for Windows.
rem
rem IMPORTANT:
rem   All files this script downloads or creates are placed under:
rem     .\tcc\
rem   relative to the CURRENT WORKING DIRECTORY.
rem
rem Self-check:
rem   GetTCC.bat --help
rem   GetTCC.bat --version
rem
rem Version:
rem   2026-06-09.4
rem ============================================================

set "app.version=2026-06-09.4"
call :SetESC app.esc

call :ParseArgs %*
set "app.rc=%errorlevel%"
if not "%app.rc%"=="0" exit /b %app.rc%
if defined app.showhelp (
    call :Usage
    exit /b 0
)
if defined app.showversion (
    echo GetTCC.bat %app.version%
    exit /b 0
)

rem ------------------------------------------------------------
rem Root layout: always current working directory + .\tcc\
rem ------------------------------------------------------------
set "app.workdir=%CD%"
set "app.root=%CD%\tcc"
set "app.downloads=%app.root%\downloads"
set "app.tmp=%app.root%\tmp"
set "app.tcc=%app.root%\tcc"
set "app.xz=%app.root%\xz"
set "app.7zip=%app.root%\7zip"

set "app.tcc.sha256.url=https://github.com/phoenixthrush/Tiny-C-Compiler/releases/download/0.9.27-win64/tcc-0.9.27-win64-bin.sha256"
set "app.tcc.zip.url=https://github.com/phoenixthrush/Tiny-C-Compiler/releases/download/0.9.27-win64/tcc-0.9.27-win64-bin.zip"
set "app.tcc.sha256=%app.downloads%\tcc-0.9.27-win64-bin.sha256"
set "app.tcc.zip=%app.downloads%\tcc-0.9.27-win64-bin.zip"

set "app.w32api.txz.url=https://github.com/phoenixthrush/Tiny-C-Compiler/releases/download/0.9.27-win64/w32api-5.0.2-mingw32-src.tar.xz"
set "app.w32api.sig.url=https://github.com/phoenixthrush/Tiny-C-Compiler/releases/download/0.9.27-win64/w32api-5.0.2-mingw32-src.tar.xz.sig"
set "app.w32api.txz=%app.downloads%\w32api-5.0.2-mingw32-src.tar.xz"
set "app.w32api.sig=%app.downloads%\w32api-5.0.2-mingw32-src.tar.xz.sig"
set "app.w32api.src=%app.tcc%\w32api-src"
set "app.w32api.tar=%app.w32api.src%\w32api-5.0.2-mingw32-src.tar"

set "app.xz.zip.url=https://github.com/tukaani-project/xz/releases/download/v5.8.3/xz-5.8.3-windows.zip"
set "app.xz.sig.url=https://github.com/tukaani-project/xz/releases/download/v5.8.3/xz-5.8.3-windows.zip.sig"
set "app.xz.zip=%app.downloads%\xz-5.8.3-windows.zip"
set "app.xz.sig=%app.downloads%\xz-5.8.3-windows.zip.sig"
set "app.xz.exe=%app.xz%\xz.exe"

set "app.w32api.sha256=b80b0c9d0158f9125e482b50fe00b70dde11d7a015ee687ca455fe2ea2ec8733"
set "app.xz.sha256=8d0048ee51177b11ef1613959c2a268c951f4e7f6fb3706e681e00e34bb6d5e3"

call :EnsureDirs
set "TEMP=%app.tmp%"
set "TMP=%app.tmp%"

if defined app.nohashcheck call :EchoYellow "hash checks are disabled for this run."

if exist "%app.tcc%\tcc.exe" (
    call :EchoGreen "TCC already available: %app.tcc%\tcc.exe"
) else (
    call :GetTCC
    set "app.rc=%errorlevel%"
    if not "%app.rc%"=="0" exit /b %app.rc%
)

call :ShowWhatWeHave
exit /b %errorlevel%

rem ============================================================
rem Usage and argument parsing
rem ============================================================

:Usage
echo GetTCC.bat %app.version%
echo.
echo Usage:
echo   GetTCC.bat [nohashcheck]
echo   GetTCC.bat help
echo   GetTCC.bat -h
echo   GetTCC.bat --help
echo   GetTCC.bat --version
echo.
echo Options:
echo   nohashcheck   Download required archives but skip .sha256/.sig downloads
echo                 and skip SHA256/GPG verification.
echo   help, -h,
echo   --help        Print this usage information and exit.
echo   --version     Print this script version and exit.
echo.
echo Layout:
echo   All downloaded and generated files are kept under:
echo     .\tcc\
echo.
echo   Main folders:
echo     .\tcc\downloads\
echo     .\tcc\tmp\
echo     .\tcc\tcc\
echo     .\tcc\tcc\w32api-src\
echo     .\tcc\xz\
echo     .\tcc\7zip\
exit /b 0

:ParseArgs
if "%~1"=="" exit /b 0
if /i "%~1"=="help" (
    set "app.showhelp=1"
    shift
    goto :ParseArgs
)
if /i "%~1"=="-h" (
    set "app.showhelp=1"
    shift
    goto :ParseArgs
)
if /i "%~1"=="--help" (
    set "app.showhelp=1"
    shift
    goto :ParseArgs
)
if /i "%~1"=="/?" (
    set "app.showhelp=1"
    shift
    goto :ParseArgs
)
if /i "%~1"=="?" (
    set "app.showhelp=1"
    shift
    goto :ParseArgs
)
if /i "%~1"=="--version" (
    set "app.showversion=1"
    shift
    goto :ParseArgs
)
if /i "%~1"=="version" (
    set "app.showversion=1"
    shift
    goto :ParseArgs
)
if /i "%~1"=="nohashcheck" (
    set "app.nohashcheck=1"
    shift
    goto :ParseArgs
)
call :EchoRed "ERROR: unrecognized argument: %~1"
echo.
call :Usage
exit /b 2

rem ============================================================
rem High-level flow
rem ============================================================

:EnsureDirs
if not exist "%app.root%\" mkdir "%app.root%" >nul 2>&1
if not exist "%app.downloads%\" mkdir "%app.downloads%" >nul 2>&1
if not exist "%app.tmp%\" mkdir "%app.tmp%" >nul 2>&1
if not exist "%app.tcc%\" mkdir "%app.tcc%" >nul 2>&1
if not exist "%app.xz%\" mkdir "%app.xz%" >nul 2>&1
if not exist "%app.7zip%\" mkdir "%app.7zip%" >nul 2>&1
exit /b 0

:GetTCC
call :DownloadTCCFiles
if errorlevel 1 exit /b %errorlevel%

call :EnsureXZ
if errorlevel 1 exit /b %errorlevel%

call :UnpackTCC
if errorlevel 1 exit /b %errorlevel%

exit /b 0

:DownloadTCCFiles
call :DownloadAndCheck "%app.tcc.zip.url%" "%app.tcc.zip%" "%app.tcc.sha256.url%" "%app.tcc.sha256%"
if errorlevel 1 exit /b %errorlevel%

call :DownloadAndCheck "%app.w32api.txz.url%" "%app.w32api.txz%" "%app.w32api.sig.url%" "%app.w32api.sig%" "%app.w32api.sha256%"
if errorlevel 1 exit /b %errorlevel%

exit /b 0

:EnsureXZ
if exist "%app.xz.exe%" (
    call :EchoGreen "xz.exe: %app.xz.exe%"
    exit /b 0
)

call :EchoYellow "xz.exe was not found under .\tcc\xz. Downloading portable xz..."
call :GetXZ
if errorlevel 1 exit /b %errorlevel%

if not exist "%app.xz.exe%" (
    call :EchoRed "ERROR: xz.exe is still unavailable after GetXZ."
    echo Expected:
    echo   "%app.xz.exe%"
    exit /b 1
)

call :EchoGreen "xz.exe: %app.xz.exe%"
exit /b 0

:GetXZ
call :DownloadAndCheck "%app.xz.zip.url%" "%app.xz.zip%" "%app.xz.sig.url%" "%app.xz.sig%" "%app.xz.sha256%"
if errorlevel 1 exit /b %errorlevel%

call :decompress "%app.xz.zip%" "%app.xz%"
if errorlevel 1 exit /b %errorlevel%

call :IsX64Windows
if errorlevel 1 (
    set "app.xz.bin=%app.xz%\bin_i686-sse2"
) else (
    set "app.xz.bin=%app.xz%\bin_x86-64"
)

if not exist "%app.xz.bin%\" (
    call :EchoRed "xz install FAILED: selected bin folder missing"
    echo   "%app.xz.bin%"
    exit /b 2
)

for %%F in ("%app.xz.bin%\*") do copy /y "%%~fF" "%app.xz%\%%~nxF" >nul 2>&1

if not exist "%app.xz.exe%" (
    call :EchoRed "xz install FAILED: xz.exe was not copied"
    echo Expected:
    echo   "%app.xz.exe%"
    exit /b 1
)

call :EchoGreen "xz bin OK: %app.xz.bin% -> %app.xz%"
exit /b 0

:UnpackTCC
echo.
echo Extracting TCC compiler...
call :decompress "%app.tcc.zip%" "%app.root%"
if errorlevel 1 exit /b %errorlevel%

if not exist "%app.w32api.src%\" mkdir "%app.w32api.src%" >nul 2>&1

echo.
echo Extracting w32api .tar.xz to .tar...
call :decompress "%app.w32api.txz%" "%app.w32api.src%"
if errorlevel 1 exit /b %errorlevel%

if not exist "%app.w32api.tar%" (
    call :EchoRed "ERROR: expected tar file was not produced:"
    echo   "%app.w32api.tar%"
    exit /b 1
)

echo.
echo Extracting w32api .tar contents...
call :decompress "%app.w32api.tar%" "%app.w32api.src%"
if errorlevel 1 exit /b %errorlevel%

exit /b 0

:ShowWhatWeHave
echo.
echo What we have now:
echo.
echo Root:
echo   "%app.root%"

echo.
if exist "%app.tcc%\" (
    echo tcc folder:
    dir "%app.tcc%" /b
) else (
    echo No tcc folder exists yet.
)

echo.
if exist "%app.w32api.src%\" (
    echo w32api source folder:
    dir "%app.w32api.src%" /b
) else (
    echo No w32api source folder exists yet.
)

echo.
if exist "%app.downloads%\" (
    echo downloads folder:
    dir "%app.downloads%" /b
) else (
    echo No downloads folder exists yet.
)

echo.
if exist "%app.xz.exe%" (
    echo xz.exe:
    echo   "%app.xz.exe%"
) else (
    echo xz.exe:
    echo   not found under "%app.xz%"
)

echo.
echo Note: w32api is currently extracted as source under:
echo   "%app.w32api.src%"
echo It is not yet installed into TCC include/lib layout.
exit /b 0

rem ============================================================
rem Download and verification
rem ============================================================

:DownloadAndCheck
set "dac_url=%~1"
set "dac_file=%~2"
set "dac_hashurl=%~3"
set "dac_hashfile=%~4"
set "dac_fallback=%~5"

if not defined dac_url (
    call :EchoRed "ERROR: DownloadAndCheck requires url."
    exit /b 2
)
if not defined dac_file (
    call :EchoRed "ERROR: DownloadAndCheck requires filepath."
    exit /b 2
)

if defined app.nohashcheck (
    if not exist "%dac_file%" (
        call :download "%dac_url%" "%dac_file%"
        if errorlevel 1 exit /b %errorlevel%
    ) else (
        call :EchoGreen "cached: %~nx2"
    )
    call :EchoYellow "hash check SKIPPED: %~nx2"
    exit /b 0
)

if not defined dac_hashurl (
    call :EchoRed "ERROR: DownloadAndCheck requires hashurl unless nohashcheck is used."
    exit /b 2
)
if not defined dac_hashfile (
    call :EchoRed "ERROR: DownloadAndCheck requires hashfilepath unless nohashcheck is used."
    exit /b 2
)

if exist "%dac_file%" if exist "%dac_hashfile%" (
    call :CheckFileIntegrity "%dac_file%" "%dac_hashfile%" "%dac_fallback%"
    if not errorlevel 1 (
        call :EchoGreen "verified cached: %~nx2"
        exit /b 0
    )
)

call :download "%dac_hashurl%" "%dac_hashfile%"
if errorlevel 1 exit /b %errorlevel%

call :download "%dac_url%" "%dac_file%"
if errorlevel 1 exit /b %errorlevel%

call :CheckFileIntegrity "%dac_file%" "%dac_hashfile%" "%dac_fallback%"
if errorlevel 1 (
    call :EchoRed "ERROR: verification failed after download: %dac_file%"
    exit /b 1
)

exit /b 0

:download
set "dl_url=%~1"
set "dl_out=%~2"

if not defined dl_url (
    call :EchoRed "ERROR: download requires a URL."
    exit /b 2
)

if not defined dl_out for %%A in ("%dl_url:/=\%") do set "dl_out=%%~nxA"
if not defined dl_out (
    call :EchoRed "ERROR: could not infer download filename."
    exit /b 2
)

for %%A in ("%dl_out%") do (
    set "dl_outdir=%%~dpA"
    set "dl_name=%%~nxA"
)

if defined dl_outdir if not exist "%dl_outdir%" mkdir "%dl_outdir%" >nul 2>&1

call :IsAppAvailable curl.exe
if errorlevel 1 (
    call :EchoRed "ERROR: curl.exe was not found."
    exit /b 3
)
set "dl_curl=%iaa.found%"

set "dl_tmp=%app.tmp%\curl-%RANDOM%-%RANDOM%.txt"
if exist "%dl_tmp%" del /q "%dl_tmp%" >nul 2>&1

"%dl_curl%" -L --fail --show-error --retry 3 --retry-delay 2 -o "%dl_out%" "%dl_url%" > "%dl_tmp%" 2>&1
set "dl_rc=%errorlevel%"

if "%dl_rc%"=="0" (
    call :EchoGreen "download OK: %dl_name%"
    del /q "%dl_tmp%" >nul 2>&1
    exit /b 0
)

call :EchoRed "download FAILED: %dl_name%"
echo URL: %dl_url%
echo To:  %dl_out%
call :PrintFile "%dl_tmp%"
del /q "%dl_tmp%" >nul 2>&1
exit /b %dl_rc%

:CheckFileIntegrity
set "cfi_file=%~1"
set "cfi_hashfile=%~2"
set "cfi_fallback=%~3"
set "cfi_ext=%~x2"

if not defined cfi_file exit /b 2
if not defined cfi_hashfile exit /b 2

if /i not "%cfi_ext%"==".sig" (
    call :hashcheck "%cfi_file%" "%cfi_hashfile%"
    exit /b %errorlevel%
)

call :IsAppAvailable gpg.exe
if not errorlevel 1 (
    call :hashcheck "%cfi_file%" "%cfi_hashfile%"
    exit /b %errorlevel%
)

if not defined cfi_fallback (
    call :EchoRed "integrity FAILED: %~nx1 ; gpg.exe missing and no fallback hash"
    exit /b 3
)

call :EchoYellow "gpg.exe was not found. Falling back to embedded SHA256..."
call :hashcheckliteral "%cfi_file%" "%cfi_fallback%"
exit /b %errorlevel%

:hashcheck
set "hc_file=%~1"
set "hc_sig=%~2"
set "hc_sigext=%~x2"

if not exist "%hc_file%" (
    call :EchoRed "ERROR: file to check does not exist:"
    echo   "%hc_file%"
    exit /b 2
)
if not exist "%hc_sig%" (
    call :EchoRed "ERROR: signature/hash file does not exist:"
    echo   "%hc_sig%"
    exit /b 2
)

if /i "%hc_sigext%"==".sha256" goto :_hashcheck_sha256
if /i "%hc_sigext%"==".sig" goto :_hashcheck_sig

call :EchoRed "ERROR: unsupported signature/hash file type:"
echo   %hc_sigext%
exit /b 2

:_hashcheck_sha256
set "hc_expected="
set "hc_actual="

for /f "usebackq tokens=1" %%H in ("%hc_sig%") do if not defined hc_expected set "hc_expected=%%H"
for /f "tokens=1" %%H in ('certutil -hashfile "%hc_file%" SHA256 2^>nul ^| findstr /R /I "^[0-9A-F][0-9A-F]*$"') do if not defined hc_actual set "hc_actual=%%H"

if not defined hc_expected (
    call :EchoRed "SHA256 FAILED: %~nx1 ; could not read expected hash"
    echo   "%hc_sig%"
    exit /b 1
)
if not defined hc_actual (
    call :EchoRed "SHA256 FAILED: %~nx1 ; could not compute hash"
    exit /b 1
)

if /i "%hc_expected%"=="%hc_actual%" (
    call :EchoGreen "SHA256 OK: %~nx1"
    exit /b 0
)

call :EchoRed "SHA256 FAILED: %~nx1"
echo   Expected: %hc_expected%
echo   Actual:   %hc_actual%
exit /b 1

:_hashcheck_sig
call :IsAppAvailable gpg.exe
if errorlevel 1 (
    call :EchoRed "signature FAILED: %~nx1 ; gpg.exe missing"
    exit /b 3
)

set "hc_gpg=%iaa.found%"
set "hc_tmp=%app.tmp%\gpg-%RANDOM%-%RANDOM%.txt"
if exist "%hc_tmp%" del /q "%hc_tmp%" >nul 2>&1

"%hc_gpg%" --batch --verify "%hc_sig%" "%hc_file%" > "%hc_tmp%" 2>&1
set "hc_rc=%errorlevel%"

if "%hc_rc%"=="0" (
    call :EchoGreen "signature OK: %~nx1"
    del /q "%hc_tmp%" >nul 2>&1
    exit /b 0
)

call :EchoRed "signature FAILED: %~nx1"
call :PrintFile "%hc_tmp%"
del /q "%hc_tmp%" >nul 2>&1
exit /b %hc_rc%

:hashcheckliteral
set "hcl_file=%~1"
set "hcl_expected=%~2"
set "hcl_actual="

if not exist "%hcl_file%" (
    call :EchoRed "SHA256 FAILED: %~nx1 ; file missing"
    echo   "%hcl_file%"
    exit /b 2
)
if not defined hcl_expected (
    call :EchoRed "SHA256 FAILED: %~nx1 ; expected hash missing"
    exit /b 2
)

for /f "tokens=1" %%H in ('certutil -hashfile "%hcl_file%" SHA256 2^>nul ^| findstr /R /I "^[0-9A-F][0-9A-F]*$"') do if not defined hcl_actual set "hcl_actual=%%H"

if not defined hcl_actual (
    call :EchoRed "SHA256 FAILED: %~nx1 ; could not compute hash"
    exit /b 1
)

if /i "%hcl_expected%"=="%hcl_actual%" (
    call :EchoGreen "SHA256 OK: %~nx1"
    exit /b 0
)

call :EchoRed "SHA256 FAILED: %~nx1"
echo   Expected: %hcl_expected%
echo   Actual:   %hcl_actual%
exit /b 1

rem ============================================================
rem Archive handling
rem ============================================================

:decompress
set "dec_src=%~1"
set "dec_dest=%~2"
set "dec_ext=%~x1"

if not exist "%dec_src%" (
    call :EchoRed "decompress FAILED: %~nx1 ; source file missing"
    echo   "%dec_src%"
    exit /b 2
)

if not defined dec_dest for %%A in ("%dec_src%") do set "dec_dest=%%~dpA"
for %%A in ("%dec_dest%.") do set "dec_dest=%%~fA"

if /i "%dec_ext%"==".zip" (
    call :unzip "%dec_src%" "%dec_dest%"
    exit /b %errorlevel%
)
if /i "%dec_ext%"==".xz" (
    call :unxz "%dec_src%" "%dec_dest%"
    exit /b %errorlevel%
)
if /i "%dec_ext%"==".tar" (
    call :untar "%dec_src%" "%dec_dest%"
    exit /b %errorlevel%
)

call :EchoRed "decompress FAILED: %~nx1 ; unsupported archive type"
exit /b 2

:unzip
set "uz_src=%~1"
set "uz_dest=%~2"

if not exist "%uz_src%" (
    call :EchoRed "unzip FAILED: %~nx1 ; source file missing"
    echo   "%uz_src%"
    exit /b 2
)
if not defined uz_dest for %%A in ("%uz_src%") do set "uz_dest=%%~dpA"
for %%A in ("%uz_dest%.") do set "uz_dest=%%~fA"
if not exist "%uz_dest%\" mkdir "%uz_dest%" >nul 2>&1

call :IsAppAvailable powershell.exe
if errorlevel 1 (
    call :EchoRed "unzip FAILED: %~nx1 ; powershell.exe not found"
    exit /b 3
)

set "uz_ps=%iaa.found%"
set "UZ_SRC=%uz_src%"
set "UZ_DEST=%uz_dest%"
set "UZ_SCRIPT=$ErrorActionPreference='Stop'; Expand-Archive -LiteralPath $env:UZ_SRC -DestinationPath $env:UZ_DEST -Force"
set "uz_tmp=%app.tmp%\powershell-unzip-%RANDOM%-%RANDOM%.txt"
if exist "%uz_tmp%" del /q "%uz_tmp%" >nul 2>&1

"%uz_ps%" -NoProfile -Sta -ExecutionPolicy Bypass -Command "%UZ_SCRIPT%" > "%uz_tmp%" 2>&1
set "uz_rc=%errorlevel%"

set "UZ_SRC="
set "UZ_DEST="
set "UZ_SCRIPT="

if "%uz_rc%"=="0" (
    call :EchoGreen "unzip OK: %~nx1 -> %uz_dest%"
    del /q "%uz_tmp%" >nul 2>&1
    exit /b 0
)

call :EchoRed "unzip FAILED: %~nx1 -> %uz_dest%"
call :PrintFile "%uz_tmp%"
del /q "%uz_tmp%" >nul 2>&1
exit /b %uz_rc%

:unxz
set "uxz_src=%~1"
set "uxz_dest=%~2"

if not exist "%uxz_src%" (
    call :EchoRed "unxz FAILED: %~nx1 ; source file missing"
    echo   "%uxz_src%"
    exit /b 2
)
if not defined uxz_dest for %%A in ("%uxz_src%") do set "uxz_dest=%%~dpA"
for %%A in ("%uxz_dest%.") do set "uxz_dest=%%~fA"
if not exist "%uxz_dest%\" mkdir "%uxz_dest%" >nul 2>&1

if not exist "%app.xz.exe%" (
    call :EchoRed "unxz FAILED: %~nx1 ; portable xz.exe not found"
    echo   "%app.xz.exe%"
    exit /b 3
)

for %%A in ("%uxz_src%") do set "uxz_outfile=%uxz_dest%\%%~nA"
set "uxz_tmp=%app.tmp%\xz-%RANDOM%-%RANDOM%.txt"
if exist "%uxz_tmp%" del /q "%uxz_tmp%" >nul 2>&1

"%app.xz.exe%" -dc "%uxz_src%" > "%uxz_outfile%" 2> "%uxz_tmp%"
set "uxz_rc=%errorlevel%"

if "%uxz_rc%"=="0" (
    call :EchoGreen "unxz OK: %~nx1 -> %uxz_outfile%"
    del /q "%uxz_tmp%" >nul 2>&1
    exit /b 0
)

del /q "%uxz_outfile%" >nul 2>&1
call :EchoRed "unxz FAILED: %~nx1 -> %uxz_outfile%"
call :PrintFile "%uxz_tmp%"
del /q "%uxz_tmp%" >nul 2>&1
exit /b %uxz_rc%

:untar
set "utar_src=%~1"
set "utar_dest=%~2"

if not exist "%utar_src%" (
    call :EchoRed "untar FAILED: %~nx1 ; source file missing"
    echo   "%utar_src%"
    exit /b 2
)
if not defined utar_dest for %%A in ("%utar_src%") do set "utar_dest=%%~dpA"
for %%A in ("%utar_dest%.") do set "utar_dest=%%~fA"
if not exist "%utar_dest%\" mkdir "%utar_dest%" >nul 2>&1

call :IsAppAvailable tar.exe
if errorlevel 1 (
    call :EchoRed "untar FAILED: %~nx1 ; tar.exe not found"
    exit /b 3
)

set "utar_tar=%iaa.found%"
set "utar_tmp=%app.tmp%\tar-%RANDOM%-%RANDOM%.txt"
if exist "%utar_tmp%" del /q "%utar_tmp%" >nul 2>&1

"%utar_tar%" -xf "%utar_src%" -C "%utar_dest%" > "%utar_tmp%" 2>&1
set "utar_rc=%errorlevel%"

if "%utar_rc%"=="0" (
    call :EchoGreen "untar OK: %~nx1 -> %utar_dest%"
    del /q "%utar_tmp%" >nul 2>&1
    exit /b 0
)

call :EchoRed "untar FAILED: %~nx1 -> %utar_dest%"
call :PrintFile "%utar_tmp%"
del /q "%utar_tmp%" >nul 2>&1
exit /b %utar_rc%

rem ============================================================
rem Utility routines
rem ============================================================

:IsAppAvailable
set "iaa.name=%~1"
set "iaa.found="

if not defined iaa.name exit /b 2

if exist "%iaa.name%" (
    for %%P in ("%iaa.name%") do set "iaa.found=%%~fP"
    exit /b 0
)

for /f "delims=" %%P in ('where "%iaa.name%" 2^>nul') do (
    set "iaa.found=%%~fP"
    exit /b 0
)

exit /b 1

:IsX64Windows
if /i "%PROCESSOR_ARCHITECTURE%"=="AMD64" exit /b 0
if /i "%PROCESSOR_ARCHITEW6432%"=="AMD64" exit /b 0
exit /b 1

:PrintFile
set "pf_file=%~1"
if not exist "%pf_file%" exit /b 0
for /f "usebackq delims=" %%L in ("%pf_file%") do echo   %%L
exit /b 0

:SetESC
set "sesc_out=%~1"
if not defined sesc_out exit /b 2
for /f %%a in ('echo prompt $E^| cmd') do set "%sesc_out%=%%a"
exit /b 0

:EchoRed
if defined app.esc (
    echo %app.esc%[91m%~1%app.esc%[0m
) else (
    echo %~1
)
exit /b 0

:EchoYellow
if defined app.esc (
    echo %app.esc%[93m%~1%app.esc%[0m
) else (
    echo %~1
)
exit /b 0

:EchoGreen
if defined app.esc (
    echo %app.esc%[92m%~1%app.esc%[0m
) else (
    echo %~1
)
exit /b 0
