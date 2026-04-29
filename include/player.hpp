#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GunningUp.h"

class Player 
{
public:
	Player();
	Player(raylib::Vector2& position, raylib::Vector2& rotation);
	~Player();

	//draws a circle with a line to show facing direction and world space position
	void draw() const;
	//updates the game object
	void update(float dt);
private:
	//Transform
	Vector2 m_position{ 0,0 };
	Vector2 m_rotation{ 0,0 };
	Vector2 m_right{ 0,0 };
	Vector2 m_up{ 0,0 };
	//game stats
	float m_speed{ 10.f };
	float m_rotSpeed{ 180.f };
};

#endif