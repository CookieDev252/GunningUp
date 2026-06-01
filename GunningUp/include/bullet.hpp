#ifndef BULLET_HPP
#define BULLET_HPP

#include "GunningUp.h"

class Line2D;



/*! \class Bullet bullet.hpp "GunningUp/include/bullet.hpp"
*	\brief this is the bullet class
*
* This class holds the basic information and behaviour of the bullet
* It also manages it's own lifespan 
*/
class Bullet {
public:
	Bullet() {};
	~Bullet() { };
	void Draw() const; ///< draws the bullet 
	void Update(float dt); ///< updates the bullet
	void Fire( Vector2 position, Vector2 direction); ///< Fires the bullet
	bool CanBeFired() { return !m_active; }; ///< checks if the bullet is ready to be fired
	bool CollidesWithCircle(Vector2 center, float size); ///< checks for collision with enemy/player
	bool CollidesWithLine(Line2D& wall); ///< checks for collision with walls
private:
	float m_speed{ 150.f }; ///< speed of the bullet per second
	Vector2 m_position{ -9999,-9999 }; ///< the position of the bullet
	Vector2 m_prevPos{ -9999,-9999 }; ///< the previous position of the bullet
	Vector2 m_collisionPoint{0,0}; ///< the point of a line on line collision (for colliding with walls)
	Vector2 m_direction{ 0,0 }; ///< normalised direction for which the bullet should travel along
	float m_lifetime{ 0 }; ///< the time since fired
	float m_maximumLifetime{ 3.f }; ///< maximum amount of time allowed from point of firing (in seconds)
	float m_size{ 1.f }; ///< size of the bullet
	int m_ricochetAmount{ 1 }; ///< number of time the bullet can ricochet
	int m_ricochetsLeft{ 1 }; ///< number of ricochets left
	bool m_active{ false }; ///< is the bullet in use
	void reset(); ///< a secret function that resets the values
};


/// <summary>
/// Draws the bullet to the screen using raylib's DrawCircle function
/// </summary>
inline void Bullet::Draw() const {
	if (m_active) DrawCircle(m_position.x, m_position.y, m_size, BLACK);
}

/// <summary>
/// Increases lifetime variables.
/// Resets the bullet if lifetime exceeds to maximum lifetime.
/// Moves the enemies position if still within lifetime, giving the bullets previous position to m_prevpos
/// </summary>
inline void Bullet::Update(float dt) {
	if (m_active) {
		m_lifetime += dt;
		if (m_lifetime > m_maximumLifetime)
		{
			reset(); 
			return;
		}
		m_prevPos = m_position;
		m_position.x += m_direction.x * m_speed * dt;
		m_position.y += m_direction.y * m_speed * dt;
	}
}


/// <summary>
/// resets all the values so it's far away from the screens area
/// this is to reduce instantiations by object pooling the bullets before hand
/// (more memory usage, but more effient overall)
/// </summary>
inline void Bullet::reset() {
	m_position = { -9999,-9999 };
	m_prevPos = { -9999,-9999 };
	m_direction = { 0,0 };
	m_active = false;
	m_lifetime = 0;
	m_ricochetsLeft = m_ricochetAmount;
}

/// <summary>
/// activates the bullet if inactive and applies a position and direction to the bullet
/// </summary>
inline void Bullet::Fire(Vector2 position, Vector2 direction)
{
	if (!m_active) {
		m_active = true; // bullet is now in an active state
		m_position = position;
		m_prevPos = position;
		m_direction = direction;
	}
}

/// <summary>
/// Checks for a collision with a circle using raylib's CheckCollisionCircleLine function
/// using the position and size of the target, and the bullets current and previous position.
/// </summary>
inline bool Bullet::CollidesWithCircle(Vector2 center, float size)
{
	if (CheckCollisionCircleLine(center, size * 0.5f, m_prevPos, m_position))
	{
		reset();
		return true;
	}
	return false;
}

/// <summary>
/// Checks for a collision with a line using the line's start and end position, 
/// and the bullets current and previous position.
/// </summary>
inline bool Bullet::CollidesWithLine(Line2D& wall)
{
	if (CheckCollisionLines(m_prevPos, m_position, wall.startPoint, wall.endPoint, &m_collisionPoint)) {
		if (m_ricochetsLeft != 0) {
			m_direction = Vector2Reflect(m_direction, Vector2Normalize(Vector2Subtract(m_position,ClosestPoint(wall.startPoint,wall.endPoint,m_position))));
			//move bullet away from wall from the point of collision by the difference in how far the bullet should've gone compared to the actual distance it went
			m_position = Vector2Add(m_collisionPoint, Vector2Scale(m_direction, Vector2Length(Vector2Subtract(m_prevPos, m_position)) - Vector2Length(Vector2Subtract(m_prevPos, m_collisionPoint))));
			m_ricochetsLeft--;
		}
		else
			reset();
		return true;
	}
	return false;

}


#endif