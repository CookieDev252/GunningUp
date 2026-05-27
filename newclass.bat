echo off

type nul > %1.cpp
type nul > %1.hpp

move ./%1.cpp ./GunningUp/src/
move ./%1.hpp ./GunningUp/include/


