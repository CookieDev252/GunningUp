#ifndef GUNNINGUP_H
#define GUNNINGUP_H

/** \file GunningUp.h
* this file holds all the includes
*/  


#include <iostream>
#include <vector>

//Raylib headers
#include "raylib-cpp.hpp"
#include "raymath.hpp"

inline Vector2 ClosestPoint(Vector2 a, Vector2 b, Vector2 p) {

	// get vector differences
	Vector2 D = Vector2Subtract(b, a);
	Vector2 AP = Vector2Subtract(p, a);

	// projected length + normalization
	float t = Vector2DotProduct(AP, D) / Vector2DotProduct(D, D);

	//clamp to [0,1]
	t = fmaxf(0, fminf(1, t));

	//calculate point
	return Vector2Add(a, Vector2Scale(D, t));
}

/** \struct Line2D
*
* connects two points
*/
struct Line2D {
public:
	Line2D(Vector2 start, Vector2 end, Color color = BLACK) {
		this->startPoint = start;
		this->endPoint = end;
		this->color = color;
		this->normal = Vector2Rotate(Vector2Normalize(Vector2Subtract(end,start)),90.f);
	}
	Vector2 startPoint;
	Vector2 endPoint;
	Vector2 normal;
	Color color;
};

//Axis DeadZones
const float m_leftStickDeadzoneX{ 0.1f };
const float m_leftStickDeadzoneY{ 0.1f };
const float m_rightStickDeadzoneX{ 0.1f };
const float m_rightStickDeadzoneY{ 0.1f };
const float m_leftTriggerDeadzone{ -0.9f };
const float m_rightTriggerDeadzone{ -0.9f };


#define TORADIANS PI/180.0f
#define XBOX_ALIAS_1 "xbox"
#define XBOX_ALIAS_2 "x-box"
#define PS_ALIAS_1   "playstation"
#define PS_ALIAS_2   "sony"
#define GLSL_VERSION 330 //Desktop Version

#include "bullet.hpp"
#include "floorGenerator.hpp"
#include "behaviourTree.hpp"
#include "entity.hpp"
#include "player.hpp"
#include "enemy.hpp"

#endif