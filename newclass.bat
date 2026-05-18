echo off

type nul > %1.cpp
type nul > %1.hpp

move ./%1.cpp ./src/
move ./%1.hpp ./include/


