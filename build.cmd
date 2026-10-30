@echo off
setlocal
rem Keep localized MSVC include diagnostics readable to CMake/Ninja dependency scanning.
chcp 65001 >nul
rem Build from any directory. Override LW_QT_ROOT / LW_NINJA / LW_BUILD_DIR if needed.
if not defined LW_QT_ROOT set "LW_QT_ROOT=D:\Qt\6.12.0\msvc2022_64"
if not defined LW_NINJA set "LW_NINJA=D:\Qt\Tools\Ninja\ninja.exe"
if not defined LW_BUILD_DIR set "LW_BUILD_DIR=%~dp0build"
set "LW_VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%LW_VSWHERE%" exit /b 2
for %%I in ("%LW_BUILD_DIR%") do set "LW_BUILD_DIR=%%~fI"
for %%I in ("%~dp0.") do set "LW_SOURCE_ROOT=%%~fI"
if /I "%LW_BUILD_DIR%"=="%LW_SOURCE_ROOT%" exit /b 2
for %%I in ("%LW_BUILD_DIR%") do if /I "%%~fI"=="%%~dI\" exit /b 2
if not exist "%LW_BUILD_DIR%" mkdir "%LW_BUILD_DIR%"
"%LW_VSWHERE%" -latest -products * -version "[18.0,19.0)" -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath > "%LW_BUILD_DIR%\vs-path.txt"
if errorlevel 1 exit /b 2
for /f "usebackq delims=" %%I in ("%LW_BUILD_DIR%\vs-path.txt") do set "LW_VS_ROOT=%%I"
if not defined LW_VS_ROOT exit /b 2
call "%LW_VS_ROOT%\VC\Auxiliary\Build\vcvars64.bat" -vcvars_ver=14.44
if errorlevel 1 exit /b 2
if not exist "%LW_QT_ROOT%\lib\cmake\Qt6\Qt6Config.cmake" exit /b 2
if not exist "%LW_NINJA%" exit /b 2
set "PATH=%LW_QT_ROOT%\bin;%PATH%"
cmake -S "%~dp0." -B "%LW_BUILD_DIR%" -G Ninja "-DCMAKE_MAKE_PROGRAM=%LW_NINJA%" "-DCMAKE_PREFIX_PATH=%LW_QT_ROOT%" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1
cmake --build "%LW_BUILD_DIR%" --parallel 4
if errorlevel 1 exit /b 1
ctest --test-dir "%LW_BUILD_DIR%" --output-on-failure
if errorlevel 1 exit /b 1
rem Start from an empty payload so DLLs from earlier deployments are never packaged.
powershell -NoProfile -Command "$b=[IO.Path]::GetFullPath($env:LW_BUILD_DIR); $d=[IO.Path]::GetFullPath((Join-Path $b 'dist')); if ([IO.Path]::GetDirectoryName($d) -ne $b) { exit 2 }; if (Test-Path -LiteralPath $d) { if ((Get-Item -LiteralPath $d).Attributes -band [IO.FileAttributes]::ReparsePoint) { exit 2 }; Remove-Item -LiteralPath $d -Recurse -Force -ErrorAction Stop }"
if errorlevel 1 exit /b 1
if exist "%LW_BUILD_DIR%\dist" exit /b 1
mkdir "%LW_BUILD_DIR%\dist"
copy /y "%LW_BUILD_DIR%\LuckyWheel.exe" "%LW_BUILD_DIR%\dist\LuckyWheel.exe" >nul
if errorlevel 1 exit /b 1
"%LW_QT_ROOT%\bin\windeployqt.exe" --release --no-translations --no-compiler-runtime --exclude-plugins qpdf --dir "%LW_BUILD_DIR%\dist" "%LW_BUILD_DIR%\dist\LuckyWheel.exe"
if errorlevel 1 exit /b 1
rem Use the existing matching redistributable DLLs; do not run a system installer.
if not defined LW_CRT_ROOT set "LW_CRT_ROOT=%LW_VS_ROOT%\VC\Redist\MSVC\14.44.35112\x64\Microsoft.VC143.CRT"
if not exist "%LW_CRT_ROOT%\msvcp140.dll" exit /b 2
if not exist "%LW_CRT_ROOT%\vcruntime140_1.dll" exit /b 2
copy /y "%LW_CRT_ROOT%\*.dll" "%LW_BUILD_DIR%\dist\" >nul
if errorlevel 1 exit /b 1
echo Build and automated tests completed. Output: %LW_BUILD_DIR%\dist\LuckyWheel.exe
exit /b 0
