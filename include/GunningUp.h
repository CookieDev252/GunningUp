#ifndef GUNNINGUP_H
#define GUNNINGUP_H

//this file holds all the includes


#include <iostream>
#include <vector>

//Raylib headers
#include "raylib-cpp.hpp"
#include "raymath.hpp"

#define TORADIANS PI/180.0f
#define XBOX_ALIAS_1 "xbox"
#define XBOX_ALIAS_2 "x-box"
#define PS_ALIAS_1   "playstation"
#define PS_ALIAS_2   "sony"
#define GLSL_VERSION 330 //Desktop Version

static std::vector<raylib::Texture2D> WallTextures;


#include "floorGenerator.hpp"
#include "player.hpp"
#include "behaviourTree.hpp"

#endif