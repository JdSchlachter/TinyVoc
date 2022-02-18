@echo off

rem * Prepare the environment:
set MINGWPATH=C:\Tools\mingw64\bin
set PATH=%MINGWPATH%\;%PATH%
mkdir  ..\build
mkdir  ..\build\Other
mkdir  ..\build\Other\Help

rem * Give out user-information:
echo.
echo. Building with MinGw
echo.
echo. Expecting MinGw in %MINGWPATH%...
echo.

rem * Build manual:
del ..\build\Other\Help\TinyVoc.html
echo ^<meta http-equiv="Content-Type" content="text/html; charset=utf-8" /^>                         >  ..\build\Other\Help\TinyVoc.html
echo ^<style title="BodyText" type="text/css"^>                                                     >>  ..\build\Other\Help\TinyVoc.html
echo  body { font-family: Segoe UI,Helvetica,sans-serif; font-weight: normal; padding-left: 20px; } >>  ..\build\Other\Help\TinyVoc.html
echo  pre { background-color: #e4e4e4; font-family: "Consolas",Consolas,monospace; padding: 20px; } >>  ..\build\Other\Help\TinyVoc.html
echo  em { color: #000099; }                                                                        >>  ..\build\Other\Help\TinyVoc.html
echo ^</style^>                                                                                     >>  ..\build\Other\Help\TinyVoc.html
C:\Tools\pandoc\pandoc ..\README.md >> ..\build\Other\Help\TinyVoc.html

rem * Copy additional files:
rem copy   ..\LICENSE     ..\build\LICENSE.txt /Y

rem * ... and build:
windres TinyVoc.rc -O coff -o TinyVoc.res
g++ -O3 -s -o ..\build\TinyVoc.exe -mwindows -static-libgcc -static-libstdc++ TinyVoc.cpp ConfigHandler.cpp Trainer.cpp TrainOpts.cpp DiacriticTranslator.cpp TinyVoc.res -lversion
del *.res

pause