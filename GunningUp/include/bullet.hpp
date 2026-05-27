#ifndef BULLET_HPP
#define BULLET_HPP

#include "GunningUp.h"

//pre define classes for functions
class Player;
class Enemy;
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
	~Bullet() { delete m_collisionPoint; };
	void Draw() const;
	void Update(float dt);
	void Fire( Vector2 position, Vector2 direction);
	bool CanBeFired() { return !m_active; };
	bool CollidesWith(Player& player);
	bool CollidesWith(Enemy& player);
	bool CollidesWith(Line2D& player);

private:
	float m_speed{ 50.f };
	Vector2 m_position{ -9999,-9999 }; ///< the position of the bullet
	Vector2 m_prevPos{ -9999,-9999 }; ///< the previous position of the bullet
	Vector2* m_collisionPoint{ nullptr }; ///< the point of a line on line collision (for colliding with walls)
	Vector2 m_direction{ 0,0 }; ///< normalised direction for which the bullet should travel along
	float m_lifetime{ 0 }; ///< the time since fired
	float m_maximumLifetime{ 3.f }; ///< maximum amount of time allowed from point of firing (in seconds)
	float m_size{ 1.f }; ///< size of the bullet
	int m_ricochetAmount{ 1 }; ///< number of time the bullet can ricochet
	int m_ricochetsLeft{ 1 }; ///< number of ricochets left
	bool m_active{ false }; ///< is the bullet in use
	void reset(); ///< a secret function that resets the values
};




#endif