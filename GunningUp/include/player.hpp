#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GunningUp.h"

class Bullet;

class Player : public Entity {
public:
	Player();
	Player(raylib::Vector2& position, raylib::Vector2& rotation);
	~Player();

	//updates the game object
	virtual void update(float dt) override;
	virtual void draw() const override;
private:
	

	float m_lookSensitivity = 720.f;
	
};



#endif