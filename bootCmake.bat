@echo off

echo checking for raylib

if exist ".\raylib\" (
    echo raylib exists
) else (
    echo raylib doesn't exist
    echo installing raylib
    git clone "https://github.com/raysan5/raylib.git"
)

echo removing build

rmdir ./build

echo making build ...

cmake -S . -B ./build

echo building executable ...

cmake --build ./build

echo done!

pause