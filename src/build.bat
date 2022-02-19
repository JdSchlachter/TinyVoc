@echo off

rem * Prepare the environment:
set MINGWPATH=C:\Tools\mingw64\bin
set PATH=%MINGWPATH%\;%PATH%

mkdir  ..\TinyVocPortable
mkdir  ..\TinyVocPortable\Other
mkdir  ..\TinyVocPortable\Other\Help

rem * Give out user-information:
echo.
echo. TinyVocPortableing with MinGw
echo.
echo. Expecting MinGw in %MINGWPATH%...
echo.

rem * TinyVocPortable manual:
del ..\TinyVocPortable\Other\Help\TinyVoc.html
echo ^<meta http-equiv="Content-Type" content="text/html; charset=utf-8" /^>                         >  ..\TinyVocPortable\Other\Help\TinyVoc.html
echo ^<style title="BodyText" type="text/css"^>                                                     >>  ..\TinyVocPortable\Other\Help\TinyVoc.html
echo  body { font-family: Segoe UI,Helvetica,sans-serif; font-weight: normal; padding-left: 20px; } >>  ..\TinyVocPortable\Other\Help\TinyVoc.html
echo  pre { background-color: #e4e4e4; font-family: "Consolas",Consolas,monospace; padding: 20px; } >>  ..\TinyVocPortable\Other\Help\TinyVoc.html
echo  em { color: #000099; }                                                                        >>  ..\TinyVocPortable\Other\Help\TinyVoc.html
echo ^</style^>                                                                                     >>  ..\TinyVocPortable\Other\Help\TinyVoc.html
C:\Tools\pandoc\pandoc ..\README.md >> ..\TinyVocPortable\Other\Help\TinyVoc.html

rem * ... and TinyVocPortable:
windres TinyVoc.rc -O coff -o TinyVoc.res
g++ -O3 -s -o ..\TinyVocPortable\TinyVoc.exe -mwindows -static-libgcc -static-libstdc++ TinyVoc.cpp ConfigHandler.cpp Trainer.cpp TrainOpts.cpp DiacriticTranslator.cpp TinyVoc.res -lversion
del *.res

pause