@echo off
setlocal enabledelayedexpansion

REM Simple Batch PDF to SVG Converter
REM Double-click to run - searches all PDF files recursively and saves to "images" folder

set pngWidth=1024

echo Simple PDF to SVG Batch Converter
echo ==================================

REM Check if converter script exists
if not exist "scripts/pdf_to_svg.bat" (
    echo Error: scripts/pdf_to_svg.bat not found in current directory!
    echo Please place both scripts in the same folder.
    pause
    exit /b 1
)



REM Create images directory
if not exist "images" mkdir "images"
echo Created/using output directory: images\
echo.

REM Initialize counter
set "count=0"
set "success=0"

REM Save starting directory (where script was called)
set "startdir=%CD%"

REM Search and convert all PDF files recursively
echo Searching for PDF files...
echo.

for /r . %%f in (*.pdf) do (
    set /a count+=1
    
    REM Get just the filename without path and extension
    for %%p in ("%%f") do set "filename=%%~np"
    
    set "output_file=images\!filename!.svg"
    
    REM Get full absolute path
    set "full_path=%%f"
    
    REM Remove the starting directory + backslash to get relative path
    set "relative_path=!full_path:%startdir%\=!"
    
    REM Call converter script silently
    call "scripts/pdf_to_svg.bat" "!relative_path!" "!output_file!"
    
    REM Check if successful
    if exist "!output_file!" (
        set /a success+=1
        echo [!count!] Converting: !filename!.pdf     SUCCESS
    ) else (
        echo [!count!] Converting: !filename!.pdf     FAILED
    )
)

for /r Images/ %%f in (*.svg) do (
    set /a count+=1
    
    REM Get just the filename without path and extension
    for %%p in ("%%f") do set "filename=%%~np"
    
    set "output_file=images\!filename!.png"
    
    REM Get full absolute path
    set "full_path=%%f"
    
    REM Remove the starting directory + backslash to get relative path
    set "relative_path=!full_path:%startdir%\=!"
    
    REM Call converter script silently
    call "scripts/pdf_to_svg.bat" "!relative_path!" "!output_file!"
    call "scripts/svg2png.bat" "!relative_path!" "!output_file!" !pngWidth!
    
    REM Check if successful
    if exist "!output_file!" (
        set /a success+=1
        echo [!count!] Converting: !filename!.pdf     SUCCESS
    ) else (
        echo [!count!] Converting: !filename!.pdf     FAILED
    )
)

echo.
echo ==================================
echo Conversion Complete!
echo ==================================
echo Total PDF files found: !count!
echo Successfully converted: !success!
echo.
echo SVG files saved to: images\
echo.
endlocal
pause
