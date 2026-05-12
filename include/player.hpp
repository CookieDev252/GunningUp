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
	Vector2 getPosition() { return m_position; }
	float getRotationHorizontal() { return m_rotation.x; }
	void MoveAndCollideWithMap(std::vector<Room>& floor);
private:
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