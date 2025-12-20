@echo off
REM =====================================================
REM Convert SVG to PNG using Inkscape
REM Usage: svg2png.bat input.svg output.png [width] [height]
REM =====================================================

REM Adds Inkscape to the path enviroment variable for this batch call session
set "PATH=C:\Program Files\Inkscape\bin;%PATH%"

if "%~1"=="" (
    echo Usage: %~nx0 input.svg output.png [width] [height]
    exit /b 1
)

set INPUT=%~1
set OUTPUT=%~2
set WIDTH=%~3
set HEIGHT=%~4

REM Check if Inkscape is installed and in PATH
where inkscape >nul 2>nul
if errorlevel 1 (
    echo Error: Inkscape is not in PATH.
    echo Add Inkscape to PATH or specify full path in this script.
    exit /b 1
)

REM Build export options
set OPTIONS=
REM If you want to preserve ratio, only set width OR height, not both
if not "%WIDTH%"=="" if "%HEIGHT%"=="" set OPTIONS=--export-width=%WIDTH%
if "%WIDTH%"=="" if not "%HEIGHT%"=="" set OPTIONS=--export-height=%HEIGHT%

REM If both are set, Inkscape will distort to fit exactly
if not "%WIDTH%"=="" if not "%HEIGHT%"=="" set OPTIONS=--export-width=%WIDTH% --export-height=%HEIGHT%


REM Run Inkscape conversion
inkscape "%INPUT%" --export-filename="%OUTPUT%" %OPTIONS%

echo Done: "%OUTPUT%"
