@echo off
if not exist build\win64 (
    mkdir build\win64
)
cd build\win64
cmake ..\..\ -G "Visual Studio 16 2019" -A x64 -DTarget=Windows -DArch=x64
cd ..\..\
pause