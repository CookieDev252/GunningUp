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


/** \class Entity entity.hpp "GunningUp/include"
*
*  \brief 
*  A base class for other entities
*  it holds basic information, like position, rotation
*  and has a collision function inside for walls
*/
class Entity {
public:
	Entity() {};
	virtual void draw() const; ///< draws the entity
	virtual void update(float dt) = 0; ///< updates the entity (behaviour goes here) 
	const Vector2 getPosition() { return m_position; }; ///< gets the position of the entity
	const float getRotationHorizontals() { return m_rotation.x; }; ///< gets the horizontal rotation of the entity
	const float getRotationVertical() { return m_rotation.y; }; ///< gets the vertical rotation of the entity
	const float getSize() { return m_size; }; ///< gets the height of the entity
	const int getHealth() { return m_health; }; ///< gets the health of the entity
	void Damage(int amount) { m_health -= amount; }; ///< applies damage to m_health
	void setPosition(Vector2& pos) { m_position = pos; }; ///< sets the position of the entity
	void setRotationHorizontal(float rotX) { m_rotation.x = rotX; }; ///< sets the horizontal rotation of the entity
	void setRotationVertical(float rotY) { m_rotation.y = rotY; }; ///< sets the vertical rotation of the entity
	void MoveAndCollideWithMap(std::vector<Line2D>& walls); ///< tests all the walls and moves the entity appropriately away from the wall
	std::vector<Bullet>& getBullets() { return m_bullets; } ///< return the bullet vector
protected:
	//Transform
	Vector2 m_position{ 0,0 }; ///< the position of the entity
	Vector2 m_rotation{ 0,0 }; ///< the rotation of the entity (x = horizontal, y = vertical)
	Vector2 m_right{ 0,0 }; ///< the right vector
	Vector2 m_up{ 0,0 }; ///< the Up vector
	//Controller Variables
	Vector2 m_controllerAxis{ 0,0 }; ///< the input axis (keyboard or left stick)
	Vector2 m_translationTotal{ 0,0 }; ///< the translation amount (not normalised)
	Vector2 m_translationUnit{ 0,0 };
	//game stats
	float m_speed{ 50.f }; ///< how fast the entity moves per second
	float m_rotSpeed{ 180.f }; ///< how fast the entity rotates per second
	float m_size{ 10.f }; ///< how big the entity's hitbox is (also the general size for drawing too
	int m_health{ 100 }; ///< how much health the entity has
	int m_weaponDamage{ 10 }; ///< how much damage the entity deals
	//gun variables
	float m_shotCooldown{ 0 }; ///< time before the next shot (0 means can shoot)
	float m_timeBetweenShots{ 2.f }; ///< the amount of time before another shot can be made
	std::vector<Bullet> m_bullets{}; ///< using object pooling to prevent lag from instantiation

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