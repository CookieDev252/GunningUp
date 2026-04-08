@echo off

echo removing build

rmdir ./build

echo making build ...

cmake -S . -B ./build

echo building executable ...

cmake --build ./build

echo done!

pause