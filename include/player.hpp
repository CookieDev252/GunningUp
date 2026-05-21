#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GunningUp.h"

class Entity;
class Player;

class Player : public Entity {
public:
	Player();
	Player(raylib::Vector2& position, raylib::Vector2& rotation);
	~Player();

	//draws a circle with a line to show facing direction and world space position
	void draw() const;
	//updates the game object
	void update(float dt);
private:
	//Axis DeadZones
	const float m_leftStickDeadzoneX = 0.1f;
	const float m_leftStickDeadzoneY = 0.1f;
	const float m_rightStickDeadzoneX = 0.1f;
	const float m_rightStickDeadzoneY = 0.1f;
	const float m_leftTriggerDeadzone = -0.9f;
	const float m_rightTriggerDeadzone = -0.9f;

	float m_lookSensitivity = 720.f;
};

#endif