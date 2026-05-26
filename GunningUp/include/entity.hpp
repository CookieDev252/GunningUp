#ifndef ENTITY_HPP
#define ENTITY_HPP

/** \file entity.hpp */

#include "GunningUp.h"

class Enemy;

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


/** \class Entity 
*
*  \brief 
*  A base class for other entities
*  it holds basic information, like position, rotation
*  and has a collision function inside for walls
*/
class Entity {
public:
	Entity() = default;
	~Entity() = default;
	virtual void draw() const;
	virtual void update(float dt) = 0;
	const Vector2 getPosition() { return m_position; };
	const float getRotationHorizontals() { return m_rotation.x; };
	const float getRotationVertical() { return m_rotation.y; };
	const float getSize() { return m_size; };
	void setPosition(Vector2& pos) { m_position = pos; };
	void setRotationHorizontal(float rotX) { m_rotation.x = rotX; };
	void setRotationVertical(float rotY) { m_rotation.y = rotY; };
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
	int m_health{ 100 };
	int m_weaponDamage{ 10 };

};

inline void Entity::draw() const
{
	DrawCircle(m_position.x, m_position.y, m_size / 2.f, { 0,255,0,255 });
	DrawLine(m_position.x, m_position.y, m_position.x + m_up.x * m_size, m_position.y - m_up.y * m_size, { 0,122,0,255 });
}

inline void Entity::MoveAndCollideWithMap(std::vector<Line2D>& walls) {
	float dX, dY, dXW, dYH, radius;

	radius = m_size * 0.5f;

	for (Line2D& wall : walls) {
		if (CheckCollisionCircleLine(m_position, m_size * 0.5f, wall.startPoint, wall.endPoint)) {
			//check what side collided
			if (Vector2DotProduct(m_position, ClosestPoint(m_position, wall.startPoint, wall.endPoint))) {
				Vector2 closestPoint = ClosestPoint(wall.startPoint, wall.endPoint, m_position);

				Vector2 directionAndLength = Vector2Scale(Vector2Normalize(Vector2Subtract(closestPoint, m_position)), m_size * 0.51f); //add a little extra on the end so it doesn't get stuck

				m_position = Vector2Add(m_position, Vector2Subtract(Vector2Subtract(closestPoint, m_position), directionAndLength));
			}
		}
	}
}



#endif