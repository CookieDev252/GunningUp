#ifndef ENTITY_HPP
#define ENTITY_HPP

/** \file entity.hpp */

#include "GunningUp.h"

/** \class Entity 
*
* A base class for other entities
*/

class Line2D;

class Entity {
public:
	Entity() {};
	~Entity() {};
	Vector2 getPosition() { return m_position; }
	float getRotationHorizontals() { return m_rotation.x; }
	float getRotationVertical() { return m_rotation.y; }
	void MoveAndCollideWithMap(std::vector<Line2D>& walls);
protected:
	//Transform
	Vector2 m_position{ 0,0 };
	Vector2 m_rotation{ 0,0 };
	Vector2 m_right{ 0,0 };
	Vector2 m_up{ 0,0 };
	//Controller Variables
	Vector2 m_controllerAxis{ 0,0 };
	Vector2 m_translationUnit{ 0,0 };
	Vector2 m_translationTotal{ 0,0 };
	//game stats
	float m_speed{ 50.f };
	float m_rotSpeed{ 180.f };
	float m_size{ 10.f };

};


#endif